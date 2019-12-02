// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeProjectile.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "MyCharacter.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACubeProjectile::ACubeProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	InitialLifeSpan = 10.0f;

	// Create Static Mesh
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMeshComponent;
	StaticMeshComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	StaticMeshComponent->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

	// Create Projectile Movement Component
	ProjectileComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Projectile Movement"));
	ProjectileComponent->InitialSpeed = 1000.0f;
	ProjectileComponent->ProjectileGravityScale = 0.0f;
	ProjectileComponent->bRotationFollowsVelocity = true;

	// Create Particle Trail Component
	ParticleTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Trail"));
	ParticleTrail->SetupAttachment(RootComponent);


	// Create Particle Explosion Component
	ParticleExplosion = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("Explosion"));
	ParticleExplosion->SetupAttachment(RootComponent);
	ParticleExplosion->SetAutoActivate(false);

	// Delegate for particle OnSystemFinished
	DestroyParticleDelegate.BindUFunction(this, "DestroyParticle");
}

// Called when the game starts or when spawned
void ACubeProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void ACubeProjectile::NotifyActorBeginOverlap(AActor* OtherActor)
{
	auto Character = Cast<AMyCharacter>(OtherActor);
	if (Character != nullptr)
	{
		Character->TakeDamage(Damage, FDamageEvent(), GetInstigatorController(), this);
		ParticleHit();
	}
}

void ACubeProjectile::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	ParticleHit();
}

void ACubeProjectile::ParticleHit()
{
	ParticleTrail->OnSystemFinished.AddUnique(DestroyParticleDelegate);
	StaticMeshComponent->SetVisibility(false);
	StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ProjectileComponent->StopMovementImmediately();
	ParticleExplosion->Activate();
}

void ACubeProjectile::DestroyParticle()
{
	Destroy();
}

// Called every frame
void ACubeProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	ParticleTrail->CustomTimeDilation = ParticleExplosion->CustomTimeDilation = GetActorTimeDilation();

}

