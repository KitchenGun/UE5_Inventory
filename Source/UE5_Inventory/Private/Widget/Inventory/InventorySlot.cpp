#include "Widget/Invnetory/InventorySlot.h"

#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Widget/Invnetory/InventoryDragDropOperation.h"
#include "Component/InventoryComponent.h"
#include "Components/Border.h"
#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "DataAsset/ItemDataAsset.h"
#include "Widget/Invnetory/InventoryGridWidget.h"
#include "Widget/Invnetory/InventoryItemDisplay.h"
#include "Widget/Invnetory/InventorySelectorWidget.h"
#include "Widget/Invnetory/InventoryWidget.h"

void UInventorySlot::SetCoordinate(const int NewRow, const int NewColumn)
{
	Row = NewRow;
	Column = NewColumn;
}

void UInventorySlot::EnableFillorReFill(const bool IsFill)
{
	if (IsFill)
	{
		Border->SetBrushColor(FLinearColor::Red);
	}
	else
	{
		Border->SetBrushColor(FLinearColor::Green);
	}
}

void UInventorySlot::DisplayDrawCanDeploy(UInventoryItemDisplay* Display)
{
	const bool IsSlotValid = InvComp->IsItemAvailableForSlot(SlotIndex, InvComp->GetCurItem()->GetItemSize(), StorageType);
	//��ġ �������� ���� ����
	Display->SetCanDeploy(IsSlotValid);

	DrawSize = { Owner->GetInventoryComponent()->GetCurItem()->GetItemSize() };

	DrawSize *= TileSize;

	Color = IsSlotValid ? ValidPlaceColor : InvalidPlaceColor;
}

void UInventorySlot::SetStorageType(EItemStorageType type)
{
	StorageType = type;
}

bool UInventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UInventoryDragDropOperation* DropWidget = Cast<UInventoryDragDropOperation>(InOperation);
	if (DropWidget == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, "Create DragDropWidget Fail");
		return false;
	}
	InvComp->SelectorVisible(ESlateVisibility::Visible);
	// const FString Message = FString::Printf(TEXT("ROW [%d] COLUMN [%d]"), Row, Column);
	// GEngine->AddOnScreenDebugMessage(-1,1.f, FColor::Black, Message);

	if (UGridPanel* GridPanel = Cast<UGridPanel>(GetParent()))
	{
		if (UInventoryItemDisplay* InventoryItemDisplay = Cast<UInventoryItemDisplay>(DropWidget->WidgetToDrag))
		{
			//�׸��� -> ���� �׸��� �̵��� ���
			if (InventoryItemDisplay->ItemData->GetStoragePos() == StorageType)//���� �������� �ű涧
			{
				if (InvComp->IsItemAvailableForSlot(SlotIndex, InvComp->GetCurItem()->GetItemSize(), Owner->GetStorageType()))
				{
					InventoryItemDisplay->SetVisibility(ESlateVisibility::Visible);
					//���� display ȸ��
					if (Owner->GetInventoryComponent()->GetCurItem()->GetIsRotate())
					{
						if (!InventoryItemDisplay->ItemData->GetIsRotate())
						{
							InventoryItemDisplay->IsRotate();
						}
					}
					else
					{
						if (InventoryItemDisplay->ItemData->GetIsRotate())
						{
							InventoryItemDisplay->IsRotate();
						}
					}
					//�׸��忡 ����
					if (UGridSlot* GridSlot = Cast<UGridSlot>(InventoryItemDisplay->Slot))
					{
						GridSlot->SetColumn(Column);
						GridSlot->SetRow(Row);
					}
					InventoryItemDisplay->SetInventoryIndex(SlotIndex);
					InventoryItemDisplay->ItemData->SetItemPos({ Column,Row });

					InvComp->SetSelectorStorage(StorageType);
					InvComp->MoveSelector(InvComp->GetCoordinateByIndex(SlotIndex, StorageType), true);
					InvComp->FillSlots(InvComp->GetCoordinateByIndex(SlotIndex, Owner->GetStorageType()), InvComp->GetCurItem()->GetItemSize(), Owner->GetStorageType());
				}
				else
				{
					bCanDraw = false;
					return false;
				}
			}
			else
			{
				//�׸��� -> �ٸ� �׸��� �̵��� ���
				if (InvComp->IsItemAvailableForSlot(SlotIndex, InvComp->GetCurItem()->GetItemSize(), StorageType))
				{
					//������ ���� ��ġ�� ���� ����
					InvComp->RemoveEquipSlotOrGrid(InventoryItemDisplay);
					//���ο� �׸��忡 ������ �߰�
					InvComp->AddItem(InvComp->GetCurItem(), SlotIndex, StorageType);
					//�κ��丮 ���÷��� �̵�
					InvComp->MoveSelector(InvComp->GetCoordinateByIndex(SlotIndex, StorageType), true);
					InvComp->FillSlots(InvComp->GetCoordinateByIndex(SlotIndex, StorageType), InvComp->GetCurItem()->GetItemSize(), StorageType);
				}
				else
				{
					bCanDraw = false;
					return false;
				}
			}
		}
	}


	//selector �ʱ�ȭ
	InvComp->GetInvWidget()->Selector->Reset();
	InvComp->SetCurItem(nullptr);

	bCanDraw = false;

	return true;
}

bool UInventorySlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragOver(InGeometry, InDragDropEvent, InOperation);

	UInventoryDragDropOperation* DropWidget = Cast<UInventoryDragDropOperation>(InOperation);

	if (DropWidget == nullptr)
	{
		return false;
	}

	//if (Owner)//���� �������� �ű涧
	{
		if (UInventoryItemDisplay* InventoryItemDisplay = Cast<UInventoryItemDisplay>(DropWidget->WidgetToDrag))
		{
			DisplayDrawCanDeploy(InventoryItemDisplay);
		}
	}

	if (UGridPanel* GridPanel = Cast<UGridPanel>(GetParent()))
	{
		//const FString Message = FString::Printf(TEXT("ROW [%d] COLUMN [%d]"), Row, Column);
		//GEngine->AddOnScreenDebugMessage(-1,1.f, FColor::Black, Message);
		bCanDraw = true;
	}

	return true;
}

int32 UInventorySlot::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
		bParentEnabled);

	if (bCanDraw)
	{
		FPaintContext Context(AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
		OnPaint(Context);

		//todo handle if box is out of Inventory widget

		UWidgetBlueprintLibrary::DrawBox(Context, { 0,0 }, DrawSize, BrushAsset, Color);

		return FMath::Max(LayerId, Context.MaxLayer);
	}

	return LayerId;
}

void UInventorySlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	bCanDraw = false;
}

void UInventorySlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	bCanDraw = false;
}
