#include "Widget/Invnetory/InventoryWidget.h"
#include "Widget/Invnetory/InventoryGridWidget.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryWidget::InitWidget()
{
}

UInventoryGridWidget* UInventoryWidget::GetGrid(EItemStorageType StorageType)
{
	return nullptr;
}

void UInventoryWidget::ItemCanSelect()
{

}

void UInventoryWidget::ItemSelect(FText ItemName, FString ItemDescription)
{

}

void UInventoryWidget::DisableSelectInfos()
{

}
