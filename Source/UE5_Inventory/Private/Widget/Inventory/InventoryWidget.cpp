#include "Widget/Invnetory/InventoryWidget.h"

#include "Component/InventoryComponent.h"
#include "Components/HorizontalBox.h"
#include "Components/Overlay.h"
#include "Components/TextBlock.h"
#include "GameFramework/Character.h"
#include "Widget/Invnetory/InventoryGridWidget.h"
#include "Widget/Invnetory/InventorySelectorWidget.h"

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	InitWidget();
}

void UInventoryWidget::InitWidget()
{
	//저장소 세팅
	SecureContainerGrid->SetStorageType(EItemStorageType::SecureContainer);
	StorageGrid->SetStorageType(EItemStorageType::Storage);
	BackpackGrid->SetStorageType(EItemStorageType::BackPack);
	//PrimaryWeapon->SlotType = EWeaponItemSlotType::PrimaryWeapon;
	//PrimaryWeapon->SlotType = EWeaponItemSlotType::PrimaryWeapon;
	//
	//inv comp 세팅
	UInventoryComponent* InvComp = Cast<ACharacter>
		(GetWorld()->GetFirstPlayerController()->GetCharacter())->
			FindComponentByClass<UInventoryComponent>();
	SecureContainerGrid->SetInventoryComponent(InvComp);
	BackpackGrid->SetInventoryComponent(InvComp);
	StorageGrid->SetInventoryComponent(InvComp);
	//PrimaryWeapon->SetInventoryComponent(InvComp);
	//PrimaryWeapon1->SetInventoryComponent(InvComp);
	//아이템 설명 초기화
	DisableSelectInfos();
}

void UInventoryWidget::ItemCanSelect()
{
	DisableSelectInfos();

	SelectInfos->SetVisibility(ESlateVisibility::Visible);

	if (!Selector->IsPointToEquipmentPlace)
	{
		//장비창을 가리키는 경우가 아니라면
		SelectInfoBox->SetVisibility(ESlateVisibility::Hidden);
		SelectButton->SetVisibility(ESlateVisibility::Visible);
		ResetButton->SetVisibility(ESlateVisibility::Hidden);
		RotateButton->SetVisibility(ESlateVisibility::Hidden);
		InteractButton->SetVisibility(ESlateVisibility::Hidden);
		DropButton->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		//숨기는 위젯
		SelectInfoBox->SetVisibility(ESlateVisibility::Hidden);
		SelectButton->SetVisibility(ESlateVisibility::Hidden);
		RotateButton->SetVisibility(ESlateVisibility::Hidden);
		//장비창을 가리키는 경우라면
		DropButton->SetVisibility(ESlateVisibility::Visible);
		InteractButton->SetVisibility(ESlateVisibility::Visible);
		ResetButton->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryWidget::ItemSelect(FText ItemName, FString ItemDescription)
{
	//전부 껐다가
	DisableSelectInfos();
	SelectInfos->SetVisibility(ESlateVisibility::Visible);
	//가시 변성
	SelectInfoBox->SetVisibility(ESlateVisibility::Visible);

	//내용 수정
	SelectItemName->SetText(ItemName);
	SelectItemDescription->SetText(FText::FromString(ItemDescription));

	SelectButton->SetVisibility(ESlateVisibility::Visible);
	InteractButton->SetVisibility(ESlateVisibility::Visible);
	RotateButton->SetVisibility(ESlateVisibility::Visible);
	DropButton->SetVisibility(ESlateVisibility::Visible);
	ResetButton->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryWidget::DisableSelectInfos()
{
	SelectInfos->SetVisibility(ESlateVisibility::Hidden);
	SelectInfoBox->SetVisibility(ESlateVisibility::Hidden);
	SelectButton->SetVisibility(ESlateVisibility::Hidden);
	InteractButton->SetVisibility(ESlateVisibility::Hidden);
	RotateButton->SetVisibility(ESlateVisibility::Hidden);
	DropButton->SetVisibility(ESlateVisibility::Hidden);
	ResetButton->SetVisibility(ESlateVisibility::Hidden);
}
