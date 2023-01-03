// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "InventoryDragDropOperation.generated.h"

/**
 * 
 */
UCLASS()
class UE5_INVENTORYUI_API UInventoryDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
public:
	UPROPERTY()
		UUserWidget* WidgetToDrag = nullptr;

	FVector2D MouseOffset = FVector2D::ZeroVector;
};
