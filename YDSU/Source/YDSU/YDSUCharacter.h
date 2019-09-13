// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "YDSUCharacter.generated.h"

UCLASS(Blueprintable)
class AYDSUCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AYDSUCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns CursorToWorld subobject **/
	FORCEINLINE class UDecalComponent* GetCursorToWorld() { return CursorToWorld; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables") float CamSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables") float CamTolerance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables") float ValueTolerance;

private:
	float AxisValueF;
	float AxisValueR;
	FVector DirectionForward;
	FVector DirectionRight;
	bool ChangeCamera;

protected:
	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	FRotator CameraRotator;
	float CameraLenght;
	bool DetachCam;
	float NewCamLagDistance;

private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** A decal that projects to the cursor location. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UDecalComponent* CursorToWorld;

	void CameraReposition(float DeltaTime);
};