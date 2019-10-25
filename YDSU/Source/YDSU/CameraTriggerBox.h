// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerBox.h"
#include "CameraTriggerBox.generated.h"

/**
 * 
 */
UCLASS()
class YDSU_API ACameraTriggerBox : public ATriggerBox
{
	GENERATED_BODY()

public:
	ACameraTriggerBox(); // Constructor
	// Gizmos
	UPROPERTY(VisibleAnywhere) class UTextRenderComponent* Label;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Mesh) USceneComponent* OriginPoint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Camera) class UCameraComponent* CamView;

	// Properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (ClampMin = "-89.0", ClampMax = "-1.0", UIMin = "-89.0", UIMax = "-1.0")) float Pitch;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (ClampMin = "-180.0", ClampMax = "180.0", UIMin = "-359.0", UIMax = "359.0")) float Yaw;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables", meta = (ClampMin = "150.0", UIMin = "150.0")) float CameraDistance;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables") bool DetachCam;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables") float CamLagSpeed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Variables") float CamLagDistance;

protected:
	virtual void OnConstruction(const FTransform& Transform) override;
};
