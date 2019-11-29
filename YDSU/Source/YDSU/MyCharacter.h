// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyCharacter.generated.h"

UCLASS(Blueprintable)
class YDSU_API AMyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyCharacter();

	// Called every frame.
	virtual void Tick(float DeltaSeconds) override;

	/** Returns TopDownCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent; }
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite) class UParticleSystemComponent* ParticleComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables") float CamSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables") float CamTolerance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables") float ValueTolerance;

	//UFUNCTION(BlueprintCallable, Category = "Test") void CopyCamValues(AMyCharacter* TheCharacter);
	UPROPERTY(BlueprintReadWrite, Category = "Variables") FRotator CameraRotator;
	UPROPERTY(BlueprintReadWrite, Category = "Variables") float CameraLenght;
	UPROPERTY(BlueprintReadWrite, Category = "Variables") float CamLagDistance;
	UPROPERTY(BlueprintReadWrite, Category = "Variables") float CamLagSpeed;
	UPROPERTY(BlueprintReadWrite, Category = "Variables") bool Locked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables") int HP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables") int FusionHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables") int BallHP;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables") int PyramidHP;

private:
	float AxisValueF;
	float AxisValueR;
	FVector DirectionForward;
	FVector DirectionRight;
	bool ChangeCamera;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Called for forwards/backward input */
	void MoveForward(float Value);


	/** Called for side to side input */
	void MoveRight(float Value);


	// Detect collisions
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;


	//bool DetachCam;
	UFUNCTION(BlueprintPure, Category = "Custom")
		FVector GetDirectionForward(float AxisValue);
	UFUNCTION(BlueprintPure, Category = "Custom")
		FVector GetDirectionRight(float AxisValue);

public:
	// Called to bind functionality to input
	//UFUNCTION(BlueprintCallable, Category = "Custom")
		void CamBoxOverLap(AActor* OtherActor);
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


private:
	/** Top down camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class UCameraComponent* TopDownCameraComponent;

	/** Camera boom positioning the camera above the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		class USpringArmComponent* CameraBoom;

	// To reposition the camera after it made contact with Camera Trigger Box
	void CameraReposition(float DeltaTime);
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
	
};
