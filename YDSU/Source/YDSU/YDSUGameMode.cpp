// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "YDSUGameMode.h"
#include "YDSUPlayerController.h"
#include "YDSUCharacter.h"
#include "UObject/ConstructorHelpers.h"

AYDSUGameMode::AYDSUGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AYDSUPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}