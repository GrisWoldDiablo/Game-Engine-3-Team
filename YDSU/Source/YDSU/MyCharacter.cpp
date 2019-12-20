// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Engine.h"
#include "CameraTriggerBox.h"
#include "Rotator.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "MyEnemyCharacter.h"
#include "MyGameInstanceCPP.h"
#include "TimerManager.h"

// Sets default values
AMyCharacter::AMyCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(50.0f, 50.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	GetCharacterMovement()->GravityScale = 2.5f;
	GetCharacterMovement()->JumpZVelocity = 1000.0f;
	GetCharacterMovement()->AirControl = 0.5f;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->bAbsoluteRotation = true; // Don't want arm to rotate when character does
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->RelativeRotation = FRotator(-60.f, 0.f, 0.f);
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Create the Particle Component
	ParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Particle Component"));
	ParticleComponent->SetupAttachment(RootComponent);
	ParticleComponent->SetAutoActivate(false);

	// Default camera repositioning values
	CamSpeed = 2.5f;
	CamTolerance = 0.5f;
	ValueTolerance = 0.2f;
	ChangeCamera = true;
	CameraRotator = FRotator(CameraBoom->RelativeRotation.Pitch, CameraBoom->RelativeRotation.Yaw, 0.0f);
	CameraLenght = CameraBoom->TargetArmLength;
	CameraBoom->bEnableCameraLag = true;
	CamLagDistance = CameraBoom->CameraLagMaxDistance = 200.0f;
	CameraBoom->CameraLagSpeed = 1.0f;
	Locked = false;

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(SetLocationHandle,
		FTimerDelegate::CreateLambda([this] {
			SetLocation = true; 
		}), 1.0f, true);
	LastFloorPosition = GetActorLocation();
	enemyIndex = 0;
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ChangeCamera)
	{
		CameraReposition(DeltaTime);
	}

	
	if (SetLocation && !GetCharacterMovement()->IsFalling() &&
		GetCharacterMovement()->MovementMode == EMovementMode::MOVE_Walking &&
		GetController()->IsPlayerController() && CanSaveFellOff)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Purple, GetActorLabel() + TEXT(":") + LastFloorPosition.ToString());
		LastFloorPosition = GetActorLocation();
		SetLocation = false;
	}
}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AMyCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AMyCharacter::MoveRight);
	PlayerInputComponent->BindAction("Target", IE_Pressed, this, &AMyCharacter::TargetEnemy);

	PlayerInputComponent->BindAction("Up", IE_Pressed, this, &AMyCharacter::ChangeEnemyUp);
	PlayerInputComponent->BindAction("Down", IE_Pressed, this, &AMyCharacter::ChangeEnemyDown);
}

void AMyCharacter::ChangeHP(int hp)
{
	if(HP < MaxHP) {
		HP = HP + hp;

	}
	else {
		HP = MaxHP;
	}

	if (HP >= MaxHP) {
		HP = MaxHP;
	}

}

void AMyCharacter::FellOfWorld()
{
	SetActorLocation(LastFloorPosition);
	GetCapsuleComponent()->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
	GetCapsuleComponent()->SetAllPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
	if (FellOffWorldParticle != nullptr) {
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FellOffWorldParticle, FTransform(LastFloorPosition));
	}
}

void AMyCharacter::MoveForward(float Value)
{

	if ((Controller != NULL))
	{
		// add movement in that direction
		AddMovementInput(GetDirectionForward(Value), Value);
	}
}

// get forward vector updated if player moved controller axis
FVector AMyCharacter::GetDirectionForward(float Value)
{
	const FRotator Rotation = CameraBoom->GetTargetRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	if (!FMath::IsNearlyEqual(Value, AxisValueF, ValueTolerance))
	{
		AxisValueF = Value;
		DirectionForward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		DirectionRight = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	}
	return DirectionForward;
}

void AMyCharacter::MoveRight(float Value)
{

	if ((Controller != NULL))
	{
		// add movement
		AddMovementInput(GetDirectionRight(Value), Value);
	}
}

// get right vector updated if player moved controller axis
FVector AMyCharacter::GetDirectionRight(float Value)
{
	const FRotator Rotation = CameraBoom->GetTargetRotation();
	const FRotator YawRotation(0, Rotation.Yaw, 0);
	if (!FMath::IsNearlyEqual(Value, AxisValueR, ValueTolerance))
	{
		AxisValueR = Value;
		DirectionRight = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		DirectionForward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	}
	return DirectionRight;
}

void AMyCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);
	CamBoxOverLap(OtherActor);
}

void AMyCharacter::CamBoxOverLap(AActor* OtherActor)
{
	auto CamTrigBox = Cast<ACameraTriggerBox>(OtherActor);
	//if (OtherActor->ActorHasTag(TEXT("CamTrigBox")))
	if (CamTrigBox != nullptr)
	{
		// DEBUG
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Touching Trigger Box");
		//ACameraTriggerBox* CamTrigBox = Cast<ACameraTriggerBox>(OtherActor);

		CameraRotator = FRotator(CamTrigBox->Pitch, CamTrigBox->Yaw, 0.0f);
		CameraLenght = CamTrigBox->CameraDistance;
		ChangeCamera = true;
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, CameraRotator.ToString());

		//DetachCam = CamTrigBox->DetachCam;
		CamLagSpeed = CameraBoom->CameraLagSpeed = CamTrigBox->CamLagSpeed;
		CamLagDistance = CamTrigBox->CamLagDistance;

	}
}

void AMyCharacter::CameraReposition(float DeltaTime)
{
	// Lerping Camera rotation
	CameraBoom->SetWorldRotation(FMath::RInterpTo(CameraBoom->GetTargetRotation(), CameraRotator, DeltaTime, CamSpeed));
	// Lerping Camera position
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, CameraLenght, DeltaTime, CamSpeed);
	// Lerping Camera follow distance
	CameraBoom->CameraLagMaxDistance = FMath::FInterpTo(CameraBoom->CameraLagMaxDistance, CamLagDistance, DeltaTime, CamSpeed);

	// Stop lerping if camera position is close enough
	if (CameraBoom->GetTargetRotation().Equals(CameraRotator, CamTolerance) &&
		FMath::IsNearlyEqual(CameraBoom->TargetArmLength, CameraLenght, CamTolerance) &&
		FMath::IsNearlyEqual(CameraBoom->CameraLagMaxDistance, CamLagDistance, CamTolerance))
	{
		// DEBUG
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "CamZeroed");
		CameraBoom->SetWorldRotation(CameraRotator);
		CameraBoom->TargetArmLength = CameraLenght;
		CameraBoom->CameraLagMaxDistance = CamLagDistance;
		ChangeCamera = false;
	}
}

void AMyCharacter::FellOutOfWorld(const UDamageType& dmgType)
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "KILL Z Overide");
	//UGameplayStatics::OpenLevel(this, FName(*GetWorld()->GetName()), false);
	FellOfWorld();
}

void AMyCharacter::TargetEnemy()
{
	
	auto MyGI = Cast<UMyGameInstanceCPP>(GetGameInstance());
	enemyIndex = 0;
	nbOfEnemies = -1;
	
	FVector currentLoc = GetActorLocation();
	TArray<TEnumAsByte<EObjectTypeQuery>> query;
	TArray<AActor*> ignore;
	TArray<AActor*> out;
	UKismetSystemLibrary::SphereOverlapActors(this, currentLoc, 500.0f, query, AMyEnemyCharacter::StaticClass(), ignore, out);

	//FString result = FString::Printf(TEXT("Hit %d actors."), out.Num());
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, result);

	if (out.IsValidIndex(0) && enemyIsTargeted == false) {
		
		auto initialTarget = Cast<AMyEnemyCharacter>(out[0]);

		if (initialTarget != nullptr) {

			if (MyGI != nullptr) {
				enemyIsTargeted = true;
				MyGI->EnemiesList.Empty();
				MyGI->EnemyCurrentlyTargeted = initialTarget;
				float initDistance = (FVector::Dist(currentLoc, initialTarget->GetActorLocation()));
				GetWorld()->GetTimerManager().SetTimer(DistanceTimer,this, &AMyCharacter::CheckingDistanceTimer, 0.2f, true);
				for (auto actor : out)
				{
					auto enemy = Cast<AMyEnemyCharacter>(actor);

					if (enemy != nullptr)
					{
						//FString result3 = FString::Printf(TEXT(" Adding found enemies"));
						//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, result3);
						MyGI->EnemiesList.Add(enemy);
						nbOfEnemies++;
						distance = (FVector::Dist(currentLoc, enemy->GetActorLocation()));


						if (distance < initDistance) {
							MyGI->EnemyCurrentlyTargeted = enemy;
						}
						else {

						}

					}
				}

				for (auto actor : MyGI->EnemiesList) {
					if (actor == MyGI->EnemyCurrentlyTargeted) {
						break;
					}
					else {
						enemyIndex++;
					}
				}
			}
		}
	}
	else {
		GetWorldTimerManager().ClearTimer(DistanceTimer);
		enemyIsTargeted = false;
	}
	

}

void AMyCharacter::ChangeEnemyUp()
{
	auto MyGI = Cast<UMyGameInstanceCPP>(GetGameInstance());
	if (MyGI != nullptr && enemyIsTargeted == true) {
		if (MyGI->EnemiesList.IsValidIndex(0)) {
			if (nbOfEnemies > enemyIndex) {
				enemyIndex++;
				if (MyGI->EnemiesList[enemyIndex] != nullptr) {
					MyGI->EnemyCurrentlyTargeted = MyGI->EnemiesList[enemyIndex];
				}
				//FString result1 = FString::Printf(TEXT("%d number of NUM function"), MyGI->EnemiesList.Num());
				//FString result2 = FString::Printf(TEXT("%d number of NBOFENEMIES"), nbOfEnemies);
				//FString result3 = FString::Printf(TEXT("%d ARRAY INDEX"), enemyIndex);
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, result1);
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, result2);
				//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, result3);
			}
			else {
				enemyIndex = 0;
				if (MyGI->EnemiesList[enemyIndex] != nullptr) {
					MyGI->EnemyCurrentlyTargeted = MyGI->EnemiesList[enemyIndex];
				}
			}

		}
		else {
			//FString result = FString::Printf(TEXT("Not valid"));
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, result);
			enemyIsTargeted = false;
			GetWorldTimerManager().ClearTimer(DistanceTimer);

		}
	}
}

void AMyCharacter::ChangeEnemyDown()
{
	auto MyGI = Cast<UMyGameInstanceCPP>(GetGameInstance());
	if (MyGI != nullptr && enemyIsTargeted == true) {
		if (MyGI->EnemiesList.IsValidIndex(0)) {
			if (enemyIndex <= 0) {
				enemyIndex = nbOfEnemies;
				if (MyGI->EnemiesList[enemyIndex] != nullptr) {
					MyGI->EnemyCurrentlyTargeted = MyGI->EnemiesList[enemyIndex];
				}
			}
			else {
				enemyIndex--;
				if (MyGI->EnemiesList[enemyIndex] != nullptr) {
					MyGI->EnemyCurrentlyTargeted = MyGI->EnemiesList[enemyIndex];
				}
			}
		}
		else {
			//FString result = FString::Printf(TEXT("Not valid"));
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, result);
			enemyIsTargeted = false;
			GetWorldTimerManager().ClearTimer(DistanceTimer);
		}
	}
}

void AMyCharacter::CheckingDistanceTimer()
{
	//FString result1 = FString::Printf(TEXT("hi"));
	//GEngine->AddOnScreenDebugMessage(-1, 0.65f, FColor::Blue, result1);
	auto MyGI = Cast<UMyGameInstanceCPP>(GetGameInstance());
	if (MyGI->EnemyCurrentlyTargeted != nullptr) {
		distance = (FVector::Dist(GetActorLocation(), MyGI->EnemyCurrentlyTargeted->GetActorLocation()));

		if (distance > 850.0f) {
			enemyIsTargeted = false;
			GetWorldTimerManager().ClearTimer(DistanceTimer);
		}
	}
}
