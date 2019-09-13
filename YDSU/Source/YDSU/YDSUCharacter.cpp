// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "YDSUCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Runtime/Engine/Classes/Engine/Engine.h"
#include "CameraTriggerBox.h"
#include "Rotator.h"

AYDSUCharacter::AYDSUCharacter()
{
	// Set size for player capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Rotate character to moving direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

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

	// Create a decal in the world to show the cursor's location
	/*CursorToWorld = CreateDefaultSubobject<UDecalComponent>("CursorToWorld");
	CursorToWorld->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UMaterial> DecalMaterialAsset(TEXT("Material'/Game/TopDownCPP/Blueprints/M_Cursor_Decal.M_Cursor_Decal'"));
	if (DecalMaterialAsset.Succeeded())
	{
		CursorToWorld->SetDecalMaterial(DecalMaterialAsset.Object);
	}
	CursorToWorld->DecalSize = FVector(16.0f, 32.0f, 32.0f);
	CursorToWorld->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());*/

	// Activate ticking in order to update the cursor every frame.
	/*PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;*/

	//Set Camera default values
	CamSpeed = 2.5f;
	CamTolerance = 1.0f;
	ValueTolerance = 0.2f;
	ChangeCamera = true;
	CameraRotator = FRotator(CameraBoom->RelativeRotation.Pitch, CameraBoom->RelativeRotation.Yaw, 0.0f);
	CameraLenght = CameraBoom->TargetArmLength;
	NewCamLagDistance = CameraBoom->CameraLagMaxDistance;
}

void AYDSUCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	PlayerInputComponent->BindAxis("MoveForward", this, &AYDSUCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AYDSUCharacter::MoveRight);

}
void AYDSUCharacter::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

	/*if (CursorToWorld != nullptr)
	{
		if (UHeadMountedDisplayFunctionLibrary::IsHeadMountedDisplayEnabled())
		{
			if (UWorld* World = GetWorld())
			{
				FHitResult HitResult;
				FCollisionQueryParams Params(NAME_None, FCollisionQueryParams::GetUnknownStatId());
				FVector StartLocation = TopDownCameraComponent->GetComponentLocation();
				FVector EndLocation = TopDownCameraComponent->GetComponentRotation().Vector() * 2000.0f;
				Params.AddIgnoredActor(this);
				World->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
				FQuat SurfaceRotation = HitResult.ImpactNormal.ToOrientationRotator().Quaternion();
				CursorToWorld->SetWorldLocationAndRotation(HitResult.Location, SurfaceRotation);
			}
		}
		else if (APlayerController* PC = Cast<APlayerController>(GetController()))
		{
			FHitResult TraceHitResult;
			PC->GetHitResultUnderCursor(ECC_Visibility, true, TraceHitResult);
			FVector CursorFV = TraceHitResult.ImpactNormal;
			FRotator CursorR = CursorFV.Rotation();
			CursorToWorld->SetWorldLocation(TraceHitResult.Location);
			CursorToWorld->SetWorldRotation(CursorR);
		}
	}*/

	if (ChangeCamera)
	{
		CameraReposition(DeltaSeconds);
	}
}

void AYDSUCharacter::MoveForward(float Value)
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

void AYDSUCharacter::MoveRight(float Value)
{
	if ((Controller != NULL))
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
}

void AYDSUCharacter::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (OtherActor->ActorHasTag(TEXT("CamTrigBox")))
	{
		// DEBUG
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Touching Trigger Box");
		ACameraTriggerBox* CamTrigBox = Cast<ACameraTriggerBox>(OtherActor);
		if (CamTrigBox != nullptr)
		{
			CameraRotator = FRotator(CamTrigBox->Pitch, CamTrigBox->Yaw, 0.0f);
			CameraLenght = CamTrigBox->CameraDistance;
			ChangeCamera = true;
			//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, CameraRotator.ToString());

			//DetachCam = CamTrigBox->DetachCam;
			CameraBoom->CameraLagSpeed = CamTrigBox->CamLagSpeed;
			NewCamLagDistance = CamTrigBox->CamLagDistance;
		}
	}
}

void AYDSUCharacter::CameraReposition(float DeltaTime)
{
	// Lerping Camera rotation
	CameraBoom->SetWorldRotation(FMath::RInterpTo(CameraBoom->GetTargetRotation(), CameraRotator, DeltaTime, CamSpeed));
	// Lerping Camera position
	CameraBoom->TargetArmLength = FMath::FInterpTo(CameraBoom->TargetArmLength, CameraLenght, DeltaTime, CamSpeed);
	// Lerping Camera follow distance
	CameraBoom->CameraLagMaxDistance = FMath::FInterpTo(CameraBoom->CameraLagMaxDistance, NewCamLagDistance, DeltaTime, CamSpeed);
	
	// Stop lerping if camera position is close enough
	if (CameraBoom->GetTargetRotation().Equals(CameraRotator, CamTolerance) &&
		FMath::IsNearlyEqual(CameraBoom->TargetArmLength, CameraLenght, CamTolerance) &&
		FMath::IsNearlyEqual(CameraBoom->CameraLagMaxDistance, NewCamLagDistance, CamTolerance))
	{
		ChangeCamera = false;
	}
}
