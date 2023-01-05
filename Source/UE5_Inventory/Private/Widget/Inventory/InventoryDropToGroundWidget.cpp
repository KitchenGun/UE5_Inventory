// Fill out your copyright notice in the Description page of Project Settings.
#include "Widget/Invnetory/InventoryDropToGroundWidget.h"

#include "Component/InventoryComponent.h"
#include "DataAsset/ItemDataAsset.h"
#include "Widget/Invnetory/InventoryDragDropOperation.h"
#include  "Widget/Invnetory/InventoryItemDisplay.h"
#include "GameFramework/Character.h"

void UInventoryDropToGroundWidget::OnDropItem(UItemDataAsset* Item)
{
	UInventoryComponent* InvComp = GetOwningPlayer()->GetCharacter()->FindComponentByClass<UInventoryComponent>();
	/*
	 * ������Ʈ���� ����� ������Ʈ ����
	 */
	InvComp->DropItem(Item->GetID(), Item->GetQuantity(), Item);
}

void UInventoryDropToGroundWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
}

bool UInventoryDropToGroundWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
	UInventoryDragDropOperation* DropWidget = Cast<UInventoryDragDropOperation>(InOperation);
	if (DropWidget == nullptr)
	{
		return false;
	}

	if (UInventoryItemDisplay* ItemDisplay = Cast<UInventoryItemDisplay>(DropWidget->WidgetToDrag))
	{
		if (!GetOwningLocalPlayer()->GetPlayerController(GetWorld())->IsLocalController())
		{
			return false;
		}
		//������ ������ ����
		UItemDataAsset* DropItemData = ItemDisplay->ItemData;// DuplicateObject(ItemDisplay->ItemData, this);

		//drop �Լ�
		OnDropItem(DropItemData);

		return true;
	}
	
	return false;
}
