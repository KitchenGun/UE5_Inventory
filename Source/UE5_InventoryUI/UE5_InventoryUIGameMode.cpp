// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5_InventoryUIGameMode.h"
#include "UE5_InventoryUICharacter.h"
#include "UObject/ConstructorHelpers.h"

AUE5_InventoryUIGameMode::AUE5_InventoryUIGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
