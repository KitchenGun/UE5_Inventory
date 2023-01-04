#include "Component/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DataAsset/ItemDataAsset.h"
#include "Engine/ActorChannel.h"
#include "Library/InventoryStructLibrary.h"
#include "Widget/Invnetory/InventoryItemDisplay.h"

//enum ��ȸ��
ENUM_RANGE_BY_COUNT(EItemStorageType, EItemStorageType::Max)

UInventoryComponent::UInventoryComponent()
{
	for (EItemStorageType Val : TEnumRange<EItemStorageType>())
	{
		ItemContainer.Add(Val);
		ItemSlots.Add(Val);
		ItemSlotMap.Add(Val);
	}
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool wroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (auto Item : TotalItemContainer)
	{
		wroteSomething |= Channel->ReplicateSubobject(Item, *Bunch, *RepFlags); // replicate the subobject
	}

	return wroteSomething;
}

void UInventoryComponent::InitSelector()
{
}

void UInventoryComponent::UpdateSelector()
{
}

bool UInventoryComponent::IsOverGrid(FIntPoint Pos, EItemStorageType& Storage)
{
	return false;
}

void UInventoryComponent::MoveSelector(FIntPoint Pos, bool IsDirect)
{
}

void UInventoryComponent::SelectorCurItemDisplayUpdate()
{
}

void UInventoryComponent::MoveEquipmentSelector(FIntPoint Input)
{
}

void UInventoryComponent::DirectMoveSelector(EItemStorageType Storage)
{
}

FIntPoint UInventoryComponent::SelectorFindItemPos(FIntPoint Pos, TPair<FIntPoint, FIntPoint>* ItemSpace,
	EItemStorageType Storage)
{
	FIntPoint result = FIntPoint{ -1 , -1 };

	for (const auto& temp : GetItemContainer(Storage))
	{
		if ((temp->ItemData->GetItemPos().X <= Pos.X && Pos.X < temp->ItemData->GetItemPos().X + temp->ItemData->GetItemSize().X)
			&& (temp->ItemData->GetItemPos().Y <= Pos.Y && Pos.Y < temp->ItemData->GetItemPos().Y + temp->ItemData->GetItemSize().Y))
		{//�����۰� ������ ��ĥ ���  �������� ������ ��ġ�� ��ȯ ���� ��Ŀ ��ü
			if (temp->ItemData->GetStoragePos() == EItemStorageType::BackPack && temp->ItemData->GetItemPos() == FIntPoint{ 0,0 })
			{
				//temp->SetSelectorVisibility(false);//���������̳��� ���
				//Pos = GetCoordinateByIndex(InventoryWidget->Selector->CurIndex, GetSelectorStorage());
			}
			else
			{
				//temp->SetSelectorVisibility(true);
				result = Pos;
				//temp �������� ��ġ , ũ��
				ItemSpace->Key = temp->ItemData->GetItemPos();
				ItemSpace->Value = temp->ItemData->GetItemSize();
			}
		}
		//else if (InventoryWidget->Selector->CurDisplayWidget != temp)
		//{
		//	temp->SetSelectorVisibility(false);
		//}
	}

	//�������� ��ã�� ���
	if (result == FIntPoint{ -1 , -1 })
	{
		return result;
	}
	else
	{
		return Pos;
	}
}

void UInventoryComponent::SelectorUIUpdate(FIntPoint Pos)
{
}

void UInventoryComponent::SelectorVisible(ESlateVisibility Val)
{
}

void UInventoryComponent::MoveGrid(int8 IsInputRight)
{
}

void UInventoryComponent::SelectItem(FVector2D Pos)
{
}

void UInventoryComponent::SelectWeaponEquipment()
{
}

void UInventoryComponent::SelectWeaponUnEquipment()
{
}

void UInventoryComponent::SelectorEquipmentPlaceClear()
{
}

void UInventoryComponent::SelectorEquipmentPlaceMark(EItemStorageType Storage)
{
}

void UInventoryComponent::SelectItemUse()
{
}

void UInventoryComponent::UseMedItem()
{
}

void UInventoryComponent::UseItemQuantityChange()
{
}

void UInventoryComponent::SelectMoveItem()
{
}

void UInventoryComponent::DeSelectItem()
{
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

bool UInventoryComponent::PickUpItem(ABaseItem* Item)
{
	//if (IsValid(Item->ItemData))
	//{
	//	return PickUpItem(Item->ItemData);
	//}
	return false;
}

bool UInventoryComponent::PickUpItem(UItemDataAsset* ItemData)
{
	/* �������� ������ ó�� �ʿ� */
	if (HasBackPack)//����
	{
		ItemData->SetStoragePos(EItemStorageType::BackPack);
		if (AddItem(ItemData, EItemStorageType::BackPack))
		{
			return true;
		}
	}
	if (HasSecureContainer)//���� �����̳�
	{
		ItemData->SetStoragePos(EItemStorageType::BackPack);
		if (AddItem(ItemData, EItemStorageType::SecureContainer))
		{
			return true;
		}
	}
	return false;
}

bool UInventoryComponent::PickUpBodyBag(TArray<UItemDataAsset*> Items)
{
	if (InventoryWidget)
	{
		//if (InventoryWidget->StorageGrid != nullptr)//������
		//{
		//	for (auto& Temp : Items)
		//	{
		//		Temp->SetStoragePos(EItemStorageType::Storage);
		//		AddItem(Temp, EItemStorageType::Storage);
		//	}
		//	InventoryWidget->StorageGrid->SetVisibility(ESlateVisibility::Visible);
		//	return true;
		//}
	}
	return false;
}

void UInventoryComponent::SearchItem()
{
	//sphere Ʈ���̽��� �ֺ��� ������ Ȯ��
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = GetOwner()->GetActorLocation();
	TArray<AActor*> ActorsIgnore;//�����ϴ� ���� �迭
	TArray<FHitResult> Hits; //�浹 ����� ������ �ִ� �迭
	//Ʈ���̽� ó��
	const bool Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, SearchRange,
		UEngineTypes::ConvertToTraceType(ECC_Camera), false, ActorsIgnore, EDrawDebugTrace::ForDuration, Hits, true, FLinearColor::Gray, FLinearColor::Blue, 60.0f);
	if (Hit)//�浹�� ���� ������
	{
		for (const FHitResult HitResult : Hits)
		{
			//if (Cast<ABaseItem>(HitResult.GetActor()))
			//{
			//	ABaseItem* Item = Cast<ABaseItem>(HitResult.GetActor());
			//	//�ߺ��� �ѱ�
			//	if (FieldDropItems.Find(Item->GetUniqueID()) != nullptr)
			//		continue;;
			//	//UID�� ���ؼ� TMap�� ����
			//	FieldDropItems.Add({ Item->GetUniqueID() , Item });
			//	//�������� ���¸� ���޹ޱ� ���ؼ� �����ۿ� ���¸� ���޹޴� �����ڷ� ���
			//	Item->AddObserver(Cast<ACharacter>(GetOwner()));
			//	//������ �߰�
			//	Item->ItemData->SetStoragePos(EItemStorageType::Storage);
			//	AddItem(Item->ItemData, EItemStorageType::Storage);
			//}
		}
	}

	//if (!FieldDropItems.IsEmpty())//������� ������ Ȱ��ȭ
	//	InventoryWidget->StorageGrid->SetVisibility(ESlateVisibility::Visible);
	//else//��������� ��Ȱ��ȭ
	//	InventoryWidget->StorageGrid->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryComponent::ClearSearchItem()
{
}

void UInventoryComponent::ItemMoveOtherGrid(UInventoryItemDisplay* CurDisplay)
{
}

void UInventoryComponent::ItemMoveSameGrid(UInventoryItemDisplay* CurDisplay)
{
}

void UInventoryComponent::RemoveItem(UItemDataAsset* ItemData)
{
}

void UInventoryComponent::GridRemoveItem(UItemDataAsset* ItemData, EItemStorageType StorageType)
{
}

void UInventoryComponent::RemoveEquipSlotOrGrid(UInventoryItemDisplay* InventoryItemDisplay)
{
}

void UInventoryComponent::ResetInventory()
{
}

void UInventoryComponent::ResetInventory(EItemStorageType Storage)
{
}

void UInventoryComponent::DropItem(int32 ID, int32 Quantity, UItemDataAsset* Item)
{
}

void UInventoryComponent::DropItem_Server_Implementation(const FItemNetInfo& ItemNetInfo)
{
}

void UInventoryComponent::DropItem_NetMultiCast_Implementation(const FItemNetInfo& ItemNetInfo)
{
}

void UInventoryComponent::DropItemAll()
{
}

void UInventoryComponent::UpdateTotalItemContainer()
{
}

void UInventoryComponent::DropItemAll_Server_Implementation(const TArray<FItemNetInfo>& Items)
{
}


bool UInventoryComponent::AddItem(UItemDataAsset* ItemDataAsset, EItemStorageType StorageType)
{
	return false;
}

bool UInventoryComponent::AddItem(UItemDataAsset* ItemDataAsset, int SlotIndex, EItemStorageType StorageType)
{
	return false;
}

bool UInventoryComponent::IsItemAvailableForSlot(const int Index, const FIntPoint& ItemSize,
	EItemStorageType StorageType) const
{
	return false;
}

int UInventoryComponent::GetSlotIndexByCoordinate(const int Column, const int Row, EItemStorageType StorageType) const
{
	return 0;
}

int UInventoryComponent::GetSlotIndexByCoordinate(const FIntPoint Pos, EItemStorageType StorageType) const
{
	return 0;
}

FIntPoint UInventoryComponent::GetCoordinateByIndex(const int Index, EItemStorageType StorageType) const
{
	return 0;
}

UInventoryItemDisplay* UInventoryComponent::FindDisplayByItemData(UItemDataAsset* Data)
{
	return nullptr;
}

void UInventoryComponent::SortItems(EItemStorageType StorageType)
{
}

void UInventoryComponent::FillSlots(const FIntPoint& StartPoint, const FIntPoint& ItemSize,
	EItemStorageType StorageType)
{
}

void UInventoryComponent::ClearSlots(const FIntPoint& StartPoint, const FIntPoint& ItemSize,
	EItemStorageType StorageType)
{
}

int UInventoryComponent::GetFirstAvailableSlotIndex(const FIntPoint& ItemSize, EItemStorageType StorageType) const
{
	return 0;
}

void UInventoryComponent::SelectorItemDisplayRoatate()
{
}

void UInventoryComponent::SetIsOpenInventory(bool IsOpen)
{
}

void UInventoryComponent::SetIsOpenInventory_Server_Implementation(bool IsOpen)
{
}


void UInventoryComponent::SetIsOpenInventory_NetMulticast_Implementation(bool IsOpen)
{
}


void UInventoryComponent::SetIsUsingMed(bool val)
{
}

bool UInventoryComponent::GetIsUsingMed()
{
	return false;
}

void UInventoryComponent::SetCurItem(UItemDataAsset* cur)
{
}

FIntPoint UInventoryComponent::GetGridSize(EItemStorageType Storage)
{
	return 0;
}

EItemStorageType UInventoryComponent::GetSelectorStorage()
{
	return EItemStorageType::None;
}

void UInventoryComponent::IA_InventoryMoveUp()
{
}

void UInventoryComponent::IA_InventoryMoveDown()
{
}

void UInventoryComponent::IA_InventoryMoveRight()
{
}

void UInventoryComponent::IA_InventoryMoveLeft()
{
}

void UInventoryComponent::IA_InventoryGridChange(float value)
{
}

void UInventoryComponent::IA_InventoryItemSelect()
{
}

void UInventoryComponent::IA_InventoryItemDeSelectOrClose()
{
}

void UInventoryComponent::IA_InventoryItemRotate()
{
}

void UInventoryComponent::IA_InventoryEquip()
{
}

void UInventoryComponent::IA_InventoryDrop()
{
}

void UInventoryComponent::IA_InventoryUsingItem()
{
}
