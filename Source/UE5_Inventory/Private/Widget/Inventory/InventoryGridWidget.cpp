// Fill out your copyright notice in the Description page of Project Settings.
#include "Widget/Invnetory/InventoryGridWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Component/InventoryComponent.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "Components/Image.h"
#include "GameFramework/Character.h"
#include "Widget/Invnetory/InventorySlot.h"

void UInventoryGridWidget::InitInventoryWidget()
{
	SecurityBorderDown->SetVisibility(ESlateVisibility::Hidden);
	SecurityBorderLeft->SetVisibility(ESlateVisibility::Hidden);
	SecurityBorderRight->SetVisibility(ESlateVisibility::Hidden);
	SecurityBorderUp->SetVisibility(ESlateVisibility::Hidden);

	InventoryGridPanel->RowFill.Empty();
	InventoryGridPanel->ColumnFill.Empty();

	if (UCanvasPanelSlot* CanvasPanelSlot = Cast<UCanvasPanelSlot>(GridPanelBorder->Slot))
	{
		CanvasPanelSlot->SetSize({ ColumnCount * TileSize,RowCount * TileSize });
	}

	int count = 0;
	for (int Row = 0; Row < RowCount; Row++)
	{
		if (InventoryGridPanel)
		{
			InventoryGridPanel->RowFill.Add(Row);
			InventoryGridPanel->SetRowFill(Row, 1.f);
		}

		for (int Column = 0; Column < ColumnCount; Column++)
		{
			if (Row == 0)
			{
				if (InventoryGridPanel)
				{
					InventoryGridPanel->ColumnFill.Add(Column);
					InventoryGridPanel->SetColumnFill(Column, 1.f);
				}
			}

			UInventorySlot* InventorySlot = WidgetTree->ConstructWidget<UInventorySlot>(InventorySlotClass);
			InventorySlot->SetCoordinate(Row, Column);
			InventorySlot->SetPadding(FMargin{ 0, 0, 0, 0 });
			InventorySlot->SetRenderTransformPivot(FVector2D{ 0,0 });
			InventorySlot->Owner = this;
			InventorySlot->InvComp = InventoryComponent;
			InventorySlot->SetIndex(count);
			InventorySlot->SetStorageType(StorageType);
			count++;

			UPanelSlot* PanelSlot = InventoryGridPanel->AddChild(InventorySlot);

			if (UGridSlot* GridSlot = Cast<UGridSlot>(PanelSlot))
			{
				GridSlot->SetColumn(Column);
				GridSlot->SetRow(Row);
				GridSlot->SetColumnSpan(1);
				GridSlot->SetRowSpan(1);
			}

			Slots.Add(InventorySlot);
			SlotmMap.Add(InventorySlot, false);
		}
	}

	if (!IsValid(InventoryComponent))
		SetInventoryComponent(Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter())->FindComponentByClass<UInventoryComponent>());

	InventoryComponent->SetItemSlots(StorageType, Slots);
	InventoryComponent->SetItemSlotMap(StorageType, SlotmMap);

	if (GetStorageType() == EItemStorageType::SecureContainer)
	{
		SecurityBorderDown->SetVisibility(ESlateVisibility::Visible);
		SecurityBorderLeft->SetVisibility(ESlateVisibility::Visible);
		SecurityBorderRight->SetVisibility(ESlateVisibility::Visible);
		SecurityBorderUp->SetVisibility(ESlateVisibility::Visible);
	}
}

void UInventoryGridWidget::SetInventoryComponent(UInventoryComponent* InvComp)
{
	InventoryComponent = InvComp;
}

EItemStorageType UInventoryGridWidget::GetStorageType()
{
	return StorageType;
}

UInventoryComponent* UInventoryGridWidget::GetInventoryComponent()
{
	return InventoryComponent;
}

void UInventoryGridWidget::SetStorageType(EItemStorageType NewStorageType)
{
	StorageType = NewStorageType;
}

