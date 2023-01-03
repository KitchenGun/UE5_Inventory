// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Inventory/InventoryDropToGroundWidget.h"

void UInventoryDropToGroundWidget::OnDropItem(UItemDataAsset* Item)
{
}

void UInventoryDropToGroundWidget::NativePreConstruct()
{
}

bool UInventoryDropToGroundWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}
