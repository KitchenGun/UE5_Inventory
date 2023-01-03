// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryDropToGroundWidget.generated.h"

class UItemDataAsset;

UCLASS()
class UE5_INVENTORYUI_API UInventoryDropToGroundWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void OnDropItem(UItemDataAsset* Item);
private:
	virtual void NativePreConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
