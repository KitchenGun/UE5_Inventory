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
		if (ItemData->GetIsRotate())
		{
			ItemData->SetIsRotate(false);
			IsRotate();
		}
		ImageSizeUpdate(ItemData);
		//텍스트 적용／／
		NameTextBlock->SetText(FText(ItemData->GetItemName()));
		//수량 표기 업데이트
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
		SlotBorder->SetBrushColor(FLinearColor{ 1, 1, 0, 0.5f });
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
	ImageSizeUpdate(ItemData);
	//BP 회전함수 호출
	ImageRotateUpdate(ItemData);

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

	//현재 클릭한 아이템 정보 넘기기
	InvComp->SetCurItem(ItemData);

	//이미 선택된 위젯이면 반환
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
	//selector에 저장
	InvComp->GetInvWidget()->Selector->SetItemDisplay(DragVisual);
	InvComp->GetInvWidget()->Selector->SetItemData(ItemData);
	//선택 위젯 업데이트
	for (auto& temp : InvComp->GetItemContainer(InvComp->GetSelectorStorage()))
	{
		temp->SetSelectorVisibility(false);
	}

	//invcomp에 저장
	WidgetDrag->DefaultDragVisual = DragVisual;
	WidgetDrag->Pivot = EDragPivot::TopLeft;
	//FVector2D(-ItemData->GetItemSize().X / 2, ItemData->GetItemSize().Y / 2);
	
	OutOperation = WidgetDrag;

	this->SetVisibility(ESlateVisibility::Hidden);
	//셀렉터
	InvComp->SelectorVisible(ESlateVisibility::Hidden);

	//그리드에 있는 경우
	if (Owner)
	{
		InvComp->ClearSlots(InvComp->GetCoordinateByIndex(SlotIndexOnInventory, Owner->GetStorageType()), ItemData->GetItemSize(), Owner->GetStorageType());
	}
}

void UInventoryItemDisplay::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	Super::NativeOnDragCancelled(InDragDropEvent, InOperation);

	//장비창의 경우 회전된 안된 객체일 경우 회전 처리
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
	//저장 공간이 그리드 공간이면 배치 불가능으로 색상 설정
	if (IsValid(InventoryItemDisplay) && ItemData->GetStoragePos() < EItemStorageType::PrimaryWeapon)
	{
		SetCantDeployColor();
	}
	//아닐 경우 일반 색상으로 지정
	else
	{
		SetInitColor();
	}
}

void UInventoryItemDisplay::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	Super::NativeOnDragLeave(InDragDropEvent, InOperation);
	//드래그 마우스가 나갈경우 원상태로 복귀 처리
	SetInitColor();
}

bool UInventoryItemDisplay::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	if (!ItemData->IsStackable())//겹치기가 불가능한 경우
		return false;
	Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);

	UInventoryDragDropOperation* DropWidget = Cast<UInventoryDragDropOperation>(InOperation);
	if (DropWidget == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Black, "Create DragDropWidget Fail");
		return false;
	}

	UInventoryItemDisplay* InventoryItemDisplay = Cast<UInventoryItemDisplay>(DropWidget->WidgetToDrag);


	//selector 초기화
	InvComp->GetInvWidget()->Selector->Reset();
	InvComp->SetCurItem(nullptr);

	//선택 위젯 업데이트
	FIntPoint Pos = InventoryItemDisplay->ItemData->GetItemPos();
	if (InvComp->GetSelectorStorage() == ItemData->GetStoragePos())
	{
		InvComp->MoveSelector(ItemData->GetItemPos(), true);
	}
	if (InventoryItemDisplay->ItemData->GetID() == ItemData->GetID())//같은 물건인 경우
	{
		if (ItemData->IsStackable())//겹치기가 가능한 경우
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


