#include "DataAsset/ItemDataAsset.h"
#include "Net/UnrealNetwork.h"

void UItemDataAsset::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UItemDataAsset, ItemInfo);
	DOREPLIFETIME(UItemDataAsset, Quantity);
}

bool UItemDataAsset::IsSupportedForNetworking() const
{
	return true;
}
