#include "Widget/Invnetory/InventoryItemDisplay.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Component/InventoryComponent.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/Character.h"
#include "Widget/Invnetory/InventoryGridWidget.h"

void UInventoryItemDisplay::Init(UItemDataAsset* BasicItemData)
{
	ItemData = DuplicateObject<UItemDataAsset>(BasicItemData, this);
	SetIsFocus(false);
	if (ItemData)
	{
		ItemIcon->SetVisibility(ESlateVisibility::Visible);
		ItemIcon->SetBrushFromSoftTexture(ItemData->GetThumbnail(), true);
		IconSizeBox->SetHeightOverride(ItemData->GetItemSize().Y * TileSize);
		IconSizeBox->SetWidthOverride(ItemData->GetItemSize().X * TileSize);
		if (ItemData->GetIsRotate())
		{
			Cast<UCanvasPanelSlot>(ItemIcon->Slot)->SetSize
			(FVector2D{ static_cast<double>(ItemData->GetItemSize().Y),static_cast<double>(ItemData->GetItemSize().X) } *TileSize);
			ItemData->SetIsRotate(false);
			IsRotate();
		}
		else
		{
			Cast<UCanvasPanelSlot>(ItemIcon->Slot)->SetSize
			(FVector2D{ static_cast<double>(ItemData->GetItemSize().X),static_cast<double>(ItemData->GetItemSize().Y) } *TileSize);
		}
		//SetSelectorVisibility(false);
		//텍스트 적용
		//if (ItemData->() == 1)
		//	NameTextBlock->SetText(FText::FromString(FString::Printf(TEXT(""))));
		//else
			NameTextBlock->SetText(FText(ItemData->GetItemName()));
		//수량 표기 업데이트
		UpdateQuantity();
	}
}
void UInventoryItemDisplay::SetInitColor()
{
	if (SlotBorder)
	{
		SlotBorder->SetBrushColor(FLinearColor{ 1, 1, 0, .5 });
	}
}

void UInventoryItemDisplay::SetCanDeployColor()
{
	if (SlotBorder)
	{
		SlotBorder->SetBrushColor(FLinearColor::Red);
	}
}


void UInventoryItemDisplay::SetCantDeployColor()
{
	if (SlotBorder)
	{
		SlotBorder->SetBrushColor(FLinearColor::Red);
	}
}

void UInventoryItemDisplay::UpdateQuantity()
{
	StackSizeTextBlock->SetText(FText::FromString(FString::Printf(TEXT("x%d"), ItemData->GetQuantity())));
	if (ItemData->GetQuantity() > 1)
	{
		StackSizeTextBlock->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		StackSizeTextBlock->SetVisibility(ESlateVisibility::Hidden);
	}

	//if (ItemData->GetId() == 1)
	//	StackSizeTextBlock->SetText(FText::FromString(FString::Printf(TEXT(""))));
}

void UInventoryItemDisplay::IsRotate()
{
	if (ItemData == nullptr)
		return;

	if (!ItemData->GetIsRotate())
	{
		ItemData->SetIsRotate(true);
	}
	else
	{
		ItemData->SetIsRotate(false);
	}

	//크기 변경
	//장비창의 아이템 디스플레이가 감춰진 상태에서 크기가 변하는 것을 막기 위한 if문
	if (ItemData->GetStoragePos() < EItemStorageType::PrimaryWeapon || GetVisibility() != ESlateVisibility::Hidden)
	{
		IconSizeBox->SetHeightOverride(ItemData->GetItemSize().Y * TileSize);
		IconSizeBox->SetWidthOverride(ItemData->GetItemSize().X * TileSize);
		ItemIcon->Brush.SetImageSize(FVector2D{ static_cast<double>(TileSize) ,static_cast<double>(TileSize) });
	}

	//회전변경
	if (!ItemData->GetIsRotate())
	{
		Cast<UCanvasPanelSlot>(ItemIcon->Slot)->SetPosition(FVector2D{ 0,0 });
		ItemIcon->SetRenderTransformAngle(0);
		ItemIcon->Brush.SetImageSize(FVector2D{ static_cast<double>(ItemData->GetItemSize().X) , static_cast<double>(ItemData->GetItemSize().Y) } *TileSize);
	}
	else
	{
		int temp = FMath::Abs(ItemData->GetItemSize().X - ItemData->GetItemSize().Y);
		//상수 값 offset 집어넣어서 회전
		Cast<UCanvasPanelSlot>(ItemIcon->Slot)->SetPosition(FVector2D{ static_cast<double>(-25 * temp),static_cast<double>(20 * temp) });
		ItemIcon->SetRenderTransformAngle(90);
		ItemIcon->Brush.SetImageSize(FVector2D{ static_cast<double>(ItemData->GetItemSize().X),static_cast<double>(ItemData->GetItemSize().Y) } *TileSize);
	}

	//변경 데이터가 넘어기기
	if (Owner)
		Owner->GetInventoryComponent()->SetCurItem(ItemData);
	else
	{
		Cast<UInventoryComponent>
			(GetWorld()->GetFirstPlayerController()->GetCharacter()->GetComponentByClass(UInventoryComponent::StaticClass()))->SetCurItem(ItemData);
	}
}

FIntPoint UInventoryItemDisplay::GetItemSize() const
{
	if (ItemData)
	{
		return ItemData->GetItemSize();
	}
	else
	{
		return FIntPoint::ZeroValue;
	}
}

void UInventoryItemDisplay::NativeConstruct()
{
	Super::NativeConstruct();
}

void UInventoryItemDisplay::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
}

void UInventoryItemDisplay::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}

FReply UInventoryItemDisplay::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UInventoryItemDisplay::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

void UInventoryItemDisplay::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
}

void UInventoryItemDisplay::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);
}

void UInventoryItemDisplay::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
}

void UInventoryItemDisplay::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
}

bool UInventoryItemDisplay::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}


