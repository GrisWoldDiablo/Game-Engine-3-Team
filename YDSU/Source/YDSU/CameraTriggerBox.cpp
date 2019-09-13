// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraTriggerBox.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ShapeComponent.h"
#include "Camera/CameraComponent.h"
#include "ConstructorHelpers.h"

ACameraTriggerBox::ACameraTriggerBox() {

	//Set default Mesh
	Origin = CreateDefaultSubobject<UStaticMeshComponent>("Origin");
	Origin->SetupAttachment(RootComponent);
	Origin->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	auto SphereAsset = ConstructorHelpers::FObjectFinder<UStaticMesh>(TEXT("StaticMesh'/Engine/BasicShapes/Sphere.Sphere'"));
	if (SphereAsset.Object != nullptr)
	{
		Origin->SetStaticMesh(SphereAsset.Object);
	}
	Origin->SetHiddenInGame(true, true);

	CamView = CreateDefaultSubobject<UCameraComponent>("CamView");
	CamView->SetupAttachment(Origin);

	Tags.Add("CamTrigBox");
	Pitch = -60.0f;
	Yaw = 0.0f;
	CameraDistance = 800.0f;
	DetachCam = false;
	CamLagSpeed = 1.0f;
	CamLagDistance = 200.0f;

}

void ACameraTriggerBox::OnConstruction(const FTransform& Transform)
{
	FVector CamViewLoc(CameraDistance * -1.0f, 0.0f, 0.0f);
	CamView->SetRelativeLocation(CamViewLoc);
	FRotator originRotation(Pitch, Yaw, 0.0f);
	Origin->SetRelativeRotation(originRotation);
}
