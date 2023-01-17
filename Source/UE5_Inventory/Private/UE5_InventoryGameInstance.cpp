#include "UE5_InventoryGameInstance.h"
#include "Library/InventoryStructLibrary.h"


UUE5_InventoryGameInstance::UUE5_InventoryGameInstance()
{
	if (DTItems!=nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Succeed"));
		UE_LOG(LogTemp, Warning, TEXT("%s"),*GetItemInfo(FName("AK47"))->ItemName.ToString());
	}
}

FItem* UUE5_InventoryGameInstance::GetItemInfo(FName Item_ID) const
{
	if(DTItems==nullptr)
	{
		UE_LOG(LogTemp,Display,TEXT("nullptr DT"));
		return nullptr;
	}
	else
	{
		return DTItems->FindRow<FItem>(Item_ID, TEXT("Can't find item"));
	}
}
