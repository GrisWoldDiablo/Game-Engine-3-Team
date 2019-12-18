// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MyEnemyCharacter.generated.h"

UCLASS()
class YDSU_API AMyEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AMyEnemyCharacter();

	UPROPERTY(VisibleAnywhere) class USplineComponent* FinisherCameraSpline;
	UPROPERTY(VisibleAnywhere) class UCameraComponent* FinisherCamera;

	// Timeline
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class UTimelineComponent* FinisherCameraTimeline;
	UPROPERTY(EditAnywhere) UCurveFloat* FinisherCameraTimelineFloatCurve;
	

protected:
	FTimerHandle TimerHandleDelay;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void OnConstruction(const FTransform& Transform) override;
	UFUNCTION(BlueprintCallable) void StartFinisherCamera();
	UFUNCTION() void RunFinisherCamera();
	UFUNCTION() void ResetFinisherCamera();
	UFUNCTION() void RunResetFinisherCamera();
	
	// Timeline
	UFUNCTION() void TimelineTick(float tickValue);
	UFUNCTION() void TimelineFinished();
	void PlayTimeline();
};
