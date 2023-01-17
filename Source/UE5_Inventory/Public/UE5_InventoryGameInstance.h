#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Library/InventoryStructLibrary.h"
#include "UE5_InventoryGameInstance.generated.h"

class UDataTable;

UCLASS()
class UE5_INVENTORY_API UUE5_InventoryGameInstance : public UGameInstance
{
	GENERATED_BODY()
		UUE5_InventoryGameInstance();

public:
	FItem* GetItemInfo(FName Item_ID) const;
	
	//DataTable
	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite)
		UDataTable* DTItems;
};
