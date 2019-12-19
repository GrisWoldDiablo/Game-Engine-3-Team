// Fill out your copyright notice in the Description page of Project Settings.


#include "MyEnemyCharacter.h"
#include "Components/SplineComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MyCharacter.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/TimelineComponent.h"
#include "MyGameInstanceCPP.h"


// Sets default values
AMyEnemyCharacter::AMyEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FinisherCameraSpline = CreateDefaultSubobject<USplineComponent>(TEXT("Finisher Camera Spline"));
	FinisherCameraSpline->SetupAttachment(RootComponent);
	FinisherCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("Finisher Camera"));
	FinisherCamera->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Timeline
	FOnTimelineFloat onTimelineCallBack;
	FOnTimelineEventStatic onTimelineFinishedCallback;
	if (FinisherCameraTimelineFloatCurve != NULL)
	{
		FinisherCameraTimeline = NewObject<UTimelineComponent>(this, FName("TimelineAnimation"));
		FinisherCameraTimeline->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		this->BlueprintCreatedComponents.Add(FinisherCameraTimeline);
		FinisherCameraTimeline->SetNetAddressable();

		FinisherCameraTimeline->SetPropertySetObject(this);
		//MyTimeline->SetDirectionPropertyName(FName("TimelineDirection"));

		FinisherCameraTimeline->SetPlaybackPosition(0.0f, false);

		onTimelineCallBack.BindUFunction(this, FName(TEXT("TimelineTick")));
		onTimelineFinishedCallback.BindUFunction(this, FName(TEXT("TimelineFinished")));
		FinisherCameraTimeline->AddInterpFloat(FinisherCameraTimelineFloatCurve, onTimelineCallBack);
		FinisherCameraTimeline->SetTimelineFinishedFunc(onTimelineFinishedCallback);

		FinisherCameraTimeline->RegisterComponent();
	}
	
}

// Called every frame
void AMyEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyEnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMyEnemyCharacter::OnConstruction(const FTransform& Transform)
{
	FVector SplinePointLocation = FinisherCameraSpline->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
	FinisherCamera->SetWorldLocation(SplinePointLocation);
	FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(FinisherCamera->GetComponentLocation(), FinisherCameraSpline->GetComponentLocation());
	FinisherCamera->SetWorldRotation(LookAtRotation);

}

UFUNCTION(BlueprintCallable) void AMyEnemyCharacter::StartFinisherCamera()
{
	auto OutActors = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), OutActors);
	for (auto Actor : OutActors) {
		Actor->CustomTimeDilation = 0.0f;
	}
	CustomTimeDilation = 0.2f;
	
	auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	PlayerController->SetViewTargetWithBlend(this, 1.0f);
	
	GetWorldTimerManager().SetTimer(TimerHandleDelay, this, &AMyEnemyCharacter::RunFinisherCamera, 1.0f, false);
}

UFUNCTION() void AMyEnemyCharacter::RunFinisherCamera()
{
	GetCharacterMovement()->DisableMovement();
	PlayTimeline();

}

UFUNCTION() void AMyEnemyCharacter::ResetFinisherCamera()
{
	auto PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	auto MyGI = Cast<UMyGameInstanceCPP>(GetGameInstance());
	PlayerController->SetViewTargetWithBlend(MyGI->CurrentCharacter, 1.0f);
	GetWorldTimerManager().SetTimer(TimerHandleDelay, this, &AMyEnemyCharacter::RunResetFinisherCamera, 1.0f, false);

}

UFUNCTION() void AMyEnemyCharacter::RunResetFinisherCamera()
{
	auto OutActors = TArray<AActor*>();
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), OutActors);
	for (auto Actor : OutActors) {
		Actor->CustomTimeDilation = 1.0f;
	}
}

UFUNCTION() void AMyEnemyCharacter::TimelineTick(float tickValue)
{
	auto Distance = FinisherCameraSpline->GetSplineLength() * tickValue;
	auto NewLocation = FinisherCameraSpline->GetLocationAtDistanceAlongSpline(Distance, ESplineCoordinateSpace::World);
	auto NewRotation = UKismetMathLibrary::FindLookAtRotation(FinisherCamera->GetComponentLocation(), FinisherCameraSpline->GetComponentLocation());
	FinisherCamera->SetWorldLocationAndRotation(NewLocation, NewRotation);
}

UFUNCTION() void AMyEnemyCharacter::TimelineFinished()
{
	ResetFinisherCamera();
}

void AMyEnemyCharacter::PlayTimeline()
{
	if (FinisherCameraTimeline != NULL)
	{
		FinisherCameraTimeline->PlayFromStart();
	}
}

