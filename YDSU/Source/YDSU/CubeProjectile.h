// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeProjectile.generated.h"

UCLASS()
class YDSU_API ACubeProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACubeProjectile();
	UPROPERTY(VisibleAnywhere) class UStaticMeshComponent* StaticMeshComponent;
	UPROPERTY(VisibleAnywhere) class UProjectileMovementComponent* ProjectileComponent;
	UPROPERTY(VisibleAnywhere) class UParticleSystemComponent* ParticleTrail;
	UPROPERTY(VisibleAnywhere) class UParticleSystemComponent* ParticleExplosion;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int Damage = 5.0f;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;
	virtual void NotifyHit(class UPrimitiveComponent* MyComp, AActor* Other, class UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit) override;

	void ParticleHit();
	UFUNCTION() void DestroyParticle();
	FScriptDelegate DestroyParticleDelegate;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
