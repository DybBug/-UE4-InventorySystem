// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "InventorySystemGameMode.h"
#include "InventorySystemPlayerController.h"
#include "InventorySystemCharacter.h"
#include "UObject/ConstructorHelpers.h"

AInventorySystemGameMode::AInventorySystemGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = AInventorySystemPlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}