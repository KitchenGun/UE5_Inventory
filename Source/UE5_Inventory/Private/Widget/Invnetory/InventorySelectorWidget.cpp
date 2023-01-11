// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget/Invnetory/InventorySelectorWidget.h"

#include "Component/InventoryComponent.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Widget/Invnetory/InventoryItemDisplay.h"
#include "Widget/Invnetory/InventoryWidget.h"
#include "Blueprint/WidgetLayoutLibrary.h"

void UInventorySelectorWidget::SetSize(int32 TileSize)
{
	if(IsValid(UWidgetLayoutLibrary::SlotAsCanvasSlot(BlinkBorder)))
	{
		UWidgetLayoutLibrary::SlotAsCanvasSlot(BlinkBorder)->SetSize(FVector2D(TileSize,TileSize));
	}
}

void UInventorySelectorWidget::SetInventoryIndex(int Index)
{
	this->CurIndex = Index;
}

void UInventorySelectorWidget::SetItemData(UItemDataAsset* Val)
{
	//아이템 선택시 설명 나오게 변경
	CurItem = Val;
}

void UInventorySelectorWidget::SetItemDisplay(UInventoryItemDisplay* Val)
{
	CurDisplayWidget = Val;
	if (IsValid(CurDisplayWidget))
		InventoryComponent->GetInvWidget()->ItemSelect(CurDisplayWidget->ItemData->GetItemName(), CurDisplayWidget->ItemData->GetDescription());
}

void UInventorySelectorWidget::SetStorage(EItemStorageType Val)
{
	//이전에 그리드 공간이 였던 경우
	if (StorageType == EItemStorageType::SecureContainer && StorageType == EItemStorageType::BackPack)
	{
		//장비창이 지금 가리키는 공간이 된 경우
		if (Val != EItemStorageType::SecureContainer && Val != EItemStorageType::BackPack)
		{
			SetIsPointToEquipmentPlace(true);
		}
	}
	StorageType = Val;
}

void UInventorySelectorWidget::Reset()
{
	SetItemData(nullptr);
	if (IsValid(CurDisplayWidget))
		CurDisplayWidget->RemoveFromParent();
	SetItemDisplay(nullptr);
}

void UInventorySelectorWidget::SetIsPointToEquipmentPlace(bool Val)
{
	IsPointToEquipmentPlace = Val;
}

bool UInventorySelectorWidget::GetIsPointToEquipmentPlace()
{
	return IsPointToEquipmentPlace;
}
