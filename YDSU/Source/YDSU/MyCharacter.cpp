// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "CameraTriggerBox.h"
#include "Rotator.h"

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
	
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ChangeCamera)
	{
		CameraReposition(DeltaTime);
	}
}

//UFUNCTION(BlueprintCallable) void AMyCharacter::CopyCamValues(AMyCharacter* TheCharacter)
//{
//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "CamRot: " + TheCharacter->GetName());
//	/*
//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "CamLenght: " + FString::FromInt(CamLenght));
//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "CamLagDist: " + FString::FromInt(CamLagDistance));
//	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, "CamLagSpeed: " + FString::FromInt(CamLagSpeed));*/
//
//	return UFUNCTION(BlueprintCallable) void();
//}

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

}


void AMyCharacter::MoveForward(float Value)
{

	if ((Controller != NULL))
	{
		// find out which way is forward
		const FRotator Rotation = CameraBoom->GetTargetRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector updated if player moved controller axis
		if (!FMath::IsNearlyEqual(Value, AxisValueF, ValueTolerance))
		{
			AxisValueF = Value;
			DirectionForward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			DirectionRight = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		}

		// add movement in that direction
		AddMovementInput(DirectionForward, Value);
	}
}

void AMyCharacter::MoveRight(float Value)
{
	if ((Controller != NULL))
	{
		if (!Locked)
		{
			// find out which way is right
			const FRotator Rotation = CameraBoom->GetTargetRotation();
			const FRotator YawRotation(0, Rotation.Yaw, 0);


			// get right vector updated if player moved controller axis
			if (!FMath::IsNearlyEqual(Value, AxisValueR, ValueTolerance))
			{
				AxisValueR = Value;
				DirectionRight = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
				DirectionForward = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
			}
			// add movement in that direction
			AddMovementInput(DirectionRight, Value);
		}
		else
		{
			AddMovementInput(this->RootComponent->GetRightVector(), Value);			
		}
	}
}

void AMyCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor->ActorHasTag(TEXT("CamTrigBox")))
	{
		// DEBUG
		//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Touching Trigger Box");
		ACameraTriggerBox* CamTrigBox = Cast<ACameraTriggerBox>(OtherActor);
		if (CamTrigBox != nullptr)
		{
			CameraRotator = FRotator(CamTrigBox->Pitch, CamTrigBox->Yaw, 0.0f);
			CameraLenght = CamTrigBox->CameraDistance;
			ChangeCamera = true;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, CameraRotator.ToString());

			//DetachCam = CamTrigBox->DetachCam;
			CamLagSpeed = CameraBoom->CameraLagSpeed = CamTrigBox->CamLagSpeed;
			CamLagDistance = CamTrigBox->CamLagDistance;
		}
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

