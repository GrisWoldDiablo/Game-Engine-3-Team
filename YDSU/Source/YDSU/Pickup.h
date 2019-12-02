// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Pickup.generated.h"

UCLASS()
class YDSU_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();
	// Animation
	UPROPERTY(EditAnywhere, Category = "Default") float RotationSpeed = 3.0f;
	UPROPERTY(EditAnywhere, Category = "Default") float WobbleSpeed = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Default") int Value = 0;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Default") class USoundBase* PickupSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly) class UStaticMeshComponent* StaticMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly) class UParticleSystemComponent* ParticleComponent;




protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

};
