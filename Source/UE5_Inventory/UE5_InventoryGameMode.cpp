// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5_InventoryGameMode.h"
#include "UE5_InventoryCharacter.h"
#include "UObject/ConstructorHelpers.h"

AUE5_InventoryGameMode::AUE5_InventoryGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
