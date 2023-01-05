#include "Widget/Invnetory/InventoryItemDisplay.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/SlateWrapperTypes.h"
#include "Component/InventoryComponent.h"
#include "Components/Border.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Components/TextBlock.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Widget/Invnetory/InventoryDragDropOperation.h"
#include "Widget/Invnetory/InventoryGridWidget.h"
#include "Widget/Invnetory/InventorySelectorWidget.h"
#include "Widget/Invnetory/InventoryWidget.h"

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
		SetSelectorVisibility(false);
		//�ؽ�Ʈ ���룯��
		//if (ItemData->() == 1)
		//	NameTextBlock->SetText(FText::FromString(FString::Printf(TEXT(""))));
		//else
			NameTextBlock->SetText(FText(ItemData->GetItemName()));
		//���� ǥ�� ������Ʈ
		UpdateQuantity();
	}
}
void UInventoryItemDisplay::SetInitColor()
{
	if (SlotBorder)
	{
		SlotBorder->SetBrushColor(BorderInitialColor);
	}
}

void UInventoryItemDisplay::SetCanDeployColor()
{
	if (SlotBorder)
	{
		SlotBorder->SetBrushColor(FLinearColor{ 1, 1, 0, .5 });
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

	//ũ�� ����
	//���â�� ������ ���÷��̰� ������ ���¿��� ũ�Ⱑ ���ϴ� ���� ���� ���� if��
	if (ItemData->GetStoragePos() < EItemStorageType::PrimaryWeapon || GetVisibility() != ESlateVisibility::Hidden)
	{
		IconSizeBox->SetHeightOverride(ItemData->GetItemSize().Y * TileSize);
		IconSizeBox->SetWidthOverride(ItemData->GetItemSize().X * TileSize);
		ItemIcon->Brush.SetImageSize(FVector2D{ static_cast<double>(TileSize) ,static_cast<double>(TileSize) });
	}

	//ȸ������
	if (!ItemData->GetIsRotate())
	{
		Cast<UCanvasPanelSlot>(ItemIcon->Slot)->SetPosition(FVector2D{ 0,0 });
		ItemIcon->SetRenderTransformAngle(0);
		ItemIcon->Brush.SetImageSize(FVector2D{ static_cast<double>(ItemData->GetItemSize().X) , static_cast<double>(ItemData->GetItemSize().Y) } *TileSize);
	}
	else
	{
		int temp = FMath::Abs(ItemData->GetItemSize().X - ItemData->GetItemSize().Y);
		//��� �� offset ����־ ȸ��
		Cast<UCanvasPanelSlot>(ItemIcon->Slot)->SetPosition(FVector2D{ static_cast<double>(-25 * temp),static_cast<double>(20 * temp) });
		ItemIcon->SetRenderTransformAngle(90);
		ItemIcon->Brush.SetImageSize(FVector2D{ static_cast<double>(ItemData->GetItemSize().X),static_cast<double>(ItemData->GetItemSize().Y) } *TileSize);
	}

	//���� �����Ͱ� �Ѿ���
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
	if (SlotBorder)
	{
		BorderInitialColor = SlotBorder->BrushColor;
	}
}

void UInventoryItemDisplay::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	IsMouseUpOnWidget = true;

	SetCanDeployColor();
}

void UInventoryItemDisplay::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	IsMouseUpOnWidget = false;
	SetInitColor();
}

FReply UInventoryItemDisplay::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	FEventReply Reply;

	Reply.NativeReply = Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
	Reply = UWidgetBlueprintLibrary::DetectDragIfPressed(InMouseEvent, this, FKey{ "LeftMouseButton" });
	
	//if (IsMouseUpOnWidget && InMouseEvent.IsMouseButtonDown(FKey{ "RightMouseButton" }))
	//{
	//	InvComp->MoveSelector(InvComp->GetCoordinateByIndex(GetInventoryIndex(), ItemData->GetStoragePos()), true);
	//	FVector2D Pos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetWorld());
	//	InvComp->SelectItem(Pos);
	//}

	return Reply.NativeReply;
}

FReply UInventoryItemDisplay::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	return FReply::Handled();
}

void UInventoryItemDisplay::NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent,
	UDragDropOperation*& OutOperation)
{
	Super::NativeOnDragDetected(InGeometry, InMouseEvent, OutOperation);
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	InvComp = Cast<ACharacter>(PC->GetCharacter())->FindComponentByClass<UInventoryComponent>();

	//���� Ŭ���� ������ ���� �ѱ��
	InvComp->SetCurItem(ItemData);

	//�̹� ���õ� �����̸� ��ȯ
	if (this->IsFocus)
		return;

	UInventoryDragDropOperation* WidgetDrag = Cast<UInventoryDragDropOperation>(UWidgetBlueprintLibrary::CreateDragDropOperation(UInventoryDragDropOperation::StaticClass()));

	if (WidgetDrag == nullptr)
	{
		return;
	}

	UInventoryItemDisplay* DragVisual = CreateWidget<UInventoryItemDisplay>(PC, GetClass());
	DragVisual->Owner = Owner;
	DragVisual->InvComp = InvComp;
	DragVisual->Init(ItemData);
	DragVisual->IsFocus = true;
	WidgetDrag->WidgetToDrag = this;
	WidgetDrag->Payload = this;
	//selector�� ����
	InvComp->GetInvWidget()->Selector->SetItemDisplay(DragVisual);
	InvComp->GetInvWidget()->Selector->SetItemData(ItemData);
	//���� ���� ������Ʈ
	for (auto& temp : InvComp->GetItemContainer(InvComp->GetSelectorStorage()))
	{
		temp->SetSelectorVisibility(false);
	}

	//invcomp�� ����
	WidgetDrag->DefaultDragVisual = DragVisual;
	WidgetDrag->Pivot = EDragPivot::MouseDown;
	//FVector2D(-ItemData->GetItemSize().X / 2, ItemData->GetItemSize().Y / 2);


	OutOperation = WidgetDrag;

	this->SetVisibility(ESlateVisibility::Hidden);
	//������
	InvComp->SelectorVisible(ESlateVisibility::Hidden);

	//�׸��忡 �ִ� ���
	if (Owner)
	{
		InvComp->ClearSlots(InvComp->GetCoordinateByIndex(SlotIndexOnInventory, Owner->GetStorageType()), ItemData->GetItemSize(), Owner->GetStorageType());
	}
}

void UInventoryItemDisplay::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	//���â�� ��� ȸ���� �ȵ� ��ü�� ��� ȸ�� ó��
	if (ItemData->GetStoragePos() >= EItemStorageType::PrimaryWeapon)
	{
		if (!ItemData->GetIsRotate())
		{
			IsRotate();
		}
	}

	this->SetVisibility(ESlateVisibility::Visible);

	SetIsFocus(false);

	SetInitColor();

	if (Owner)
	{
		InvComp->FillSlots(InvComp->GetCoordinateByIndex(SlotIndexOnInventory, Owner->GetStorageType()), ItemData->GetItemSize(), Owner->GetStorageType());
	}

	StackSizeTextBlock->SetText(FText::FromString(FString::Printf(TEXT("x%d"), ItemData->GetQuantity())));
}

void UInventoryItemDisplay::NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragEnter(InGeometry, InDragDropEvent, InOperation);
	UInventoryDragDropOperation* DropWidget = Cast<UInventoryDragDropOperation>(InOperation);
	if (DropWidget == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, "Create DragDropWidget Fail");
	}

	UInventoryItemDisplay* InventoryItemDisplay = Cast<UInventoryItemDisplay>(DropWidget->WidgetToDrag);
	//���� ������ �׸��� �����̸� ��ġ �Ұ������� ���� ����
	if (IsValid(InventoryItemDisplay) && ItemData->GetStoragePos() < EItemStorageType::PrimaryWeapon)
	{
		SetCantDeployColor();
	}
	//�ƴ� ��� �Ϲ� �������� ����
	else
	{
		SetInitColor();
	}
}

void UInventoryItemDisplay::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	//�巡�� ���콺�� ������� �����·� ���� ó��
	SetInitColor();
}

bool UInventoryItemDisplay::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (!ItemData->IsStackable())//��ġ�Ⱑ �Ұ����� ���
		return false;
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UInventoryDragDropOperation* DropWidget = Cast<UInventoryDragDropOperation>(InOperation);
	if (DropWidget == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, "Create DragDropWidget Fail");
		return false;
	}

	UInventoryItemDisplay* InventoryItemDisplay = Cast<UInventoryItemDisplay>(DropWidget->WidgetToDrag);


	//selector �ʱ�ȭ
	InvComp->GetInvWidget()->Selector->Reset();
	InvComp->SetCurItem(nullptr);

	//���� ���� ������Ʈ
	FIntPoint Pos = InventoryItemDisplay->ItemData->GetItemPos();
	if (InvComp->GetSelectorStorage() == ItemData->GetStoragePos())
	{
		InvComp->MoveSelector(ItemData->GetItemPos(), true);
	}
	if (InventoryItemDisplay->ItemData->GetID() == ItemData->GetID())//���� ������ ���
	{
		if (ItemData->IsStackable())//��ġ�Ⱑ ������ ���
		{
			int32 AllQuantity = ItemData->GetQuantity() + InventoryItemDisplay->ItemData->GetQuantity();
			if (AllQuantity <= ItemData->GetMaxQuantity())
			{
				ItemData->AddQuantity(InventoryItemDisplay->ItemData->GetQuantity());
				StackSizeTextBlock->SetText(FText::FromString(FString::Printf(TEXT("x%d"), ItemData->GetQuantity())));
				return true;
			}
			else
			{
				int32 extraQuantity = AllQuantity - ItemData->GetMaxQuantity();
				ItemData->SetQuantity(ItemData->GetMaxQuantity());
				InventoryItemDisplay->ItemData->SetQuantity(extraQuantity);
				StackSizeTextBlock->SetText(FText::FromString(FString::Printf(TEXT("x%d"), ItemData->GetQuantity())));
				return false;
			}
		}
	}
	return false;
}


