// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h"

class UCanvasPanel;
class UHorizontalBox;
class UVerticalBox;

UCLASS()
class UE5_INVENTORY_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* MainInventoryCanvas;
	UPROPERTY(meta = (BindWidget))
		UHorizontalBox* MainHorizontalBox;
	/*¿Â∫Ò ¿Â¬¯*/
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* EquipVerticalBox;
};
