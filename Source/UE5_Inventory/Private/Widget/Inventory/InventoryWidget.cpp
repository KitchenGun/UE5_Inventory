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
	//����� ����
	SecureContainerGrid->SetStorageType(EItemStorageType::SecureContainer);
	StorageGrid->SetStorageType(EItemStorageType::Storage);
	BackpackGrid->SetStorageType(EItemStorageType::BackPack);
	//PrimaryWeapon->SlotType = EWeaponItemSlotType::PrimaryWeapon;
	//PrimaryWeapon->SlotType = EWeaponItemSlotType::PrimaryWeapon;
	//
	//inv comp ����
	UInventoryComponent* InvComp = Cast<ACharacter>
		(GetWorld()->GetFirstPlayerController()->GetCharacter())->
			FindComponentByClass<UInventoryComponent>();
	SecureContainerGrid->SetInventoryComponent(InvComp);
	BackpackGrid->SetInventoryComponent(InvComp);
	StorageGrid->SetInventoryComponent(InvComp);
	//PrimaryWeapon->SetInventoryComponent(InvComp);
	//PrimaryWeapon1->SetInventoryComponent(InvComp);
	//������ ���� �ʱ�ȭ
	DisableSelectInfos();
}

void UInventoryWidget::ItemCanSelect()
{
	DisableSelectInfos();

	SelectInfos->SetVisibility(ESlateVisibility::Visible);

	if (!Selector->IsPointToEquipmentPlace)
	{
		//���â�� ����Ű�� ��찡 �ƴ϶��
		SelectInfoBox->SetVisibility(ESlateVisibility::Hidden);
		SelectButton->SetVisibility(ESlateVisibility::Visible);
		ResetButton->SetVisibility(ESlateVisibility::Hidden);
		RotateButton->SetVisibility(ESlateVisibility::Hidden);
		InteractButton->SetVisibility(ESlateVisibility::Hidden);
		DropButton->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		//����� ����
		SelectInfoBox->SetVisibility(ESlateVisibility::Hidden);
		SelectButton->SetVisibility(ESlateVisibility::Hidden);
		RotateButton->SetVisibility(ESlateVisibility::Hidden);
		//���â�� ����Ű�� �����
		DropButton->SetVisibility(ESlateVisibility::Visible);
		InteractButton->SetVisibility(ESlateVisibility::Visible);
		ResetButton->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UInventoryWidget::ItemSelect(FText ItemName, FString ItemDescription)
{
	//���� ���ٰ�
	DisableSelectInfos();
	SelectInfos->SetVisibility(ESlateVisibility::Visible);
	//���� ����
	SelectInfoBox->SetVisibility(ESlateVisibility::Visible);

	//���� ����
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
