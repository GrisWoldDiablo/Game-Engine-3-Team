// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstanceCPP.generated.h"

/**
 * 
 */
UCLASS()
class YDSU_API UMyGameInstanceCPP : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) class AMyCharacter* CurrentCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) class AMyEnemyCharacter* EnemyCurrentlyTargeted;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite) TArray<AMyEnemyCharacter*> EnemiesList;
};
