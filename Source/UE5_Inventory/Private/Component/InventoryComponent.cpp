#include "Component/InventoryComponent.h"

#include "Components/GridPanel.h"
#include "Components/GridSlot.h"
#include "DataAsset/ItemDataAsset.h"
#include "Engine/ActorChannel.h"
#include "GameFramework/Character.h"
#include "Item/BaseItem.h"
#include "Net/UnrealNetwork.h"
#include "Widget/Invnetory/InventoryGridWidget.h"
#include "Widget/Invnetory/InventoryItemDisplay.h"
#include "Widget/Invnetory/InventorySelectorWidget.h"
#include "Widget/Invnetory/InventorySlot.h"
#include "Widget/Invnetory/InventoryWidget.h"

//enum ��ȸ��
ENUM_RANGE_BY_COUNT(EItemStorageType, EItemStorageType::Max)

UInventoryComponent::UInventoryComponent()
{
	
}

void UInventoryComponent::Init()
{
	for (EItemStorageType Val : TEnumRange<EItemStorageType>())
	{
		ItemContainer.Add(Val);
		ItemSlots.Add(Val);
		ItemSlotMap.Add(Val);
	}
	InitSelector();
}

void UInventoryComponent::InitSelector()
{
	InventoryWidget->Selector->InventoryComponent = this;
	/*�κ��丮 ������ ���ñ� �ʱ�ȭ*/
	//������Ʈ ����
	SelectorPos = 0;
	CurItem = nullptr;
	//������ ���� ����
	InventoryWidget->Selector->Reset();
	InventoryWidget->Selector->SetStorage(EItemStorageType::BackPack);
	InventoryWidget->Selector->SetInventoryIndex(0);
	const FIntPoint ItemStartPoint = GetCoordinateByIndex(0, EItemStorageType::BackPack);
	InventoryWidget->GetGrid(EItemStorageType::SecureContainer)->InventoryGridPanel->
		AddChildToGrid(InventoryWidget->Selector, ItemStartPoint.Y, ItemStartPoint.X);
	//size �ʱ�ȭ
	int32 TileSize = InventoryWidget->BackpackGrid->TileSize;
	//���� ������ �������� ���� ���
	InventoryWidget->Selector->SetSize(TileSize);
	//�������� ���� ���
	for (auto& temp : GetItemContainer(EItemStorageType::SecureContainer))
	{
		if (GetSlotIndexByCoordinate(temp->ItemData->GetItemPos(), EItemStorageType::SecureContainer) == 0)
		{
			InventoryWidget->Selector->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	//���â ����Ű�� �ִ� ��� ���
	InventoryWidget->Selector->SetIsPointToEquipmentPlace(false);

	//���� ���� ���̰� �ϱ�
	InventoryWidget->Selector->SetVisibility(ESlateVisibility::Visible);
}

void UInventoryComponent::UpdateSelector()
{
	float WaitTime = 0.3f;
	GetWorld()->GetTimerManager().SetTimer(UpdateSelectorHandler, FTimerDelegate::CreateLambda([&]()
		{
			IsAlreadyUpdate = false;
		}), WaitTime, false);
}

bool UInventoryComponent::IsOverGrid(FIntPoint Pos, EItemStorageType& Storage)
{
	int32 Max_X = GetGridSize(Storage).X;
	int32 Max_Y = GetGridSize(Storage).Y;

	int32 Min_X = 0;
	int32 Min_Y = 0;

	//���濡 ���ϴ� ��� ���� �����̳� ������ ���� �ʵ��� ����ó��
	if (Storage == EItemStorageType::BackPack)
	{
		Min_X = GetGridSize(EItemStorageType::SecureContainer).X;
		Min_Y = GetGridSize(EItemStorageType::SecureContainer).Y;
	}
	//���� Ȯ��
	if ((Min_X <= Pos.X && Pos.X < Max_X) && (Min_Y <= Pos.Y && Pos.Y < Max_Y))
	{
		return true;
	}

	//���� �׸��忡���� ���� ������ �������� �ٸ� �׸��忡 ���� ���(����)
	if (Storage == EItemStorageType::BackPack)
	{
		if (0 <= Pos.X && Pos.X < Max_X && 0 <= Pos.Y && Pos.Y < Max_Y)//���� ��
		{
			return true;
		}
	}
	//���� �׸��忡���� ���� ������ �������� �ٸ� �׸��忡 ���� ���(���Ȼ���)
	else if (Storage == EItemStorageType::SecureContainer)
	{
		if (Max_X <= Pos.X || Max_Y <= Pos.Y)
		{
			if (Pos.X < GetGridSize(EItemStorageType::BackPack).X || Pos.Y < GetGridSize(EItemStorageType::BackPack).Y)
			{
				MoveGrid(1);
				//move selector �Լ����� ����ϱ����ؼ� �߰������� ����
				Storage = EItemStorageType::BackPack;
				return true;
			}
		}
	}

	/*�߿�*/
	//�׸��忡 ����� ���� ������ ��ġ�� �ű�� ���

	//x���� �ƴ� Y���� ��� �ش������ �������� ��ȯ
	if (Min_X <= Pos.X && Pos.X < Max_X)
		return false;

	////���� & ���� �����̳�
	//if (GetSelectorStorage() == EItemStorageType::SecureContainer || GetSelectorStorage() == EItemStorageType::BackPack)
	//{
	//	//���â �������� ������ ���
	//	if (Min_X > Pos.X)
	//	{
	//		//���â ����Ų�ٰ� ���� ����
	//		InventoryWidget->Selector->SetIsPointToEquipmentPlace(true);
	//		//����Ű�� ���� ����
	//		InventoryWidget->Selector->SetStorage(EItemStorageType::PrimaryWeapon);
	//		InventoryWidget->PrimaryWeapon->WeaponDisplayWidget->SetVisibility(ESlateVisibility::Visible);
	//		InventoryWidget->PrimaryWeapon->WeaponDisplayWidget->SetSelectorVisibility(true);
	//		if (!InventoryWidget->PrimaryWeapon->GetIsSlotEmpty())
	//			InventoryWidget->ItemCanSelect();
	//
	//		//������ �ʵ��� ����
	//		InventoryWidget->Selector->SetVisibility(ESlateVisibility::Hidden);
	//	}
	//	else if (Max_X <= Pos.X)
	//	{
	//		//����Ű�� ���� ����
	//		//Storage = EItemStorageType::Storage;
	//		MoveGrid(1);
	//	}
	//
	//}
	////�׿��� ��� ����
	//else if (GetSelectorStorage() == EItemStorageType::Storage)
	//{
	//	//���â �������� ������ ���
	//	if (Min_X > Pos.X)
	//	{
	//		//Storage = EItemStorageType::BackPack;
	//		MoveGrid(-1);
	//	}
	//}
	return false;
}

void UInventoryComponent::MoveSelector(FIntPoint Pos, bool IsDirect)
{
	if (IsAlreadyUpdate)
		return;
	IsAlreadyUpdate = true;
	EItemStorageType Storage = InventoryWidget->Selector->StorageType;
	UpdateSelector();
	//size ���ϱ�
	const int32 TileSize = InventoryWidget->BackpackGrid->TileSize;
	//temp �������� ��ġ , ũ�� ������ ����
	TPair<FIntPoint, FIntPoint>SelectorItemInfo{ FIntPoint{ 0,0 }, FIntPoint{ 0,0 } };
	TPair<FIntPoint, FIntPoint>PrevSelectorItemInfo{ FIntPoint{ 0,0 }, FIntPoint{ 0,0 } };
	TPair<FIntPoint, FIntPoint>* ItemSpace = &SelectorItemInfo;
	TPair<FIntPoint, FIntPoint>* PrevItemSpace = &PrevSelectorItemInfo;
	//�׸��� ������ �ƴϸ� ���� ó���ϰ� ��ȯ
	if (InventoryWidget->Selector->GetIsPointToEquipmentPlace())
	{
		MoveEquipmentSelector(Pos);
		return;
	}
	/*�׸��� �� �̵� ó��*/
	//�������� �ε��� ����&�̵��ؾ��ϴ� �� �Է�
	if (IsDirect)
	{
		//���� ���� ���
		if (!IsOverGrid(Pos, Storage))
			return;
		//Ž��
		Pos = SelectorFindItemPos(Pos, ItemSpace, Storage);
		//��ǥ ����
		SelectorPos = GetSlotIndexByCoordinate(Pos.X, Pos.Y, Storage);
		//�̵�
		DirectMoveSelector(Storage);

		//UI ������Ʈ
		SelectorUIUpdate(Pos);
		SelectorCurItemDisplayUpdate();
	}
	//�̵� ��ǲ�� ���ؼ� �̵��ϴ� ���
	else
	{
		//Ŀ�� �̵� ����
		FIntPoint Dir = Pos;
		//Ŀ���� ������ �浹 ���� Ȯ��
		//FIntPoint TempPrevSelectorPos = SelectorFindItemPos(GetCoordinateByIndex(SelectorPos, Storage), PrevItemSpace, Storage);
		FIntPoint TempSelectorPos = SelectorFindItemPos(GetCoordinateByIndex(SelectorPos, Storage), ItemSpace, Storage);

		//�������� �����ȿ� Ŀ���� �ִ� ��� �̵����� ����
		if (InventoryWidget->Selector->GetVisibility() == ESlateVisibility::Hidden && InventoryWidget->Selector->CurDisplayWidget == nullptr)
		{
			if (Dir.X == 0)//���� �̵�
			{
				if (Dir.Y > 0)
				{
					Pos.Y = (ItemSpace->Key.Y + ItemSpace->Value.Y) - GetCoordinateByIndex(SelectorPos, Storage).Y;
				}
				else
				{
					Pos.Y = (ItemSpace->Key.Y - GetCoordinateByIndex(SelectorPos, Storage).Y) + Dir.Y;
				}
			}
			else if (Dir.Y == 0)//�¿� �̵�
			{
				if (Dir.X > 0)
				{
					Pos.X = (ItemSpace->Key.X + ItemSpace->Value.X) - GetCoordinateByIndex(SelectorPos, Storage).X;
				}
				else
				{
					Pos.X = (ItemSpace->Key.X - GetCoordinateByIndex(SelectorPos, Storage).X) + Dir.X;
				}
			}
		}
		//���°��� ������ġ ���ϱ�
		Pos = GetCoordinateByIndex(SelectorPos, Storage) + Pos;
		//���� Ȯ��
		if (!IsOverGrid(Pos, Storage))
			return;
		//��ǥ ����
		SelectorPos = GetSlotIndexByCoordinate(Pos, Storage);
		//�̵�
		DirectMoveSelector(Storage);
		//UI ������Ʈ
		SelectorUIUpdate(SelectorFindItemPos(GetCoordinateByIndex(SelectorPos, Storage), ItemSpace, Storage));
		SelectorCurItemDisplayUpdate();
	}
}

void UInventoryComponent::SelectorCurItemDisplayUpdate()
{
	//���� ������ �������� ������� ������ ��ȯó��
	if (!IsValid(InventoryWidget->Selector->CurDisplayWidget))
		return;

	if (InventoryWidget->Selector->CurDisplayWidget->GetCanDeploy())
	{
		InventoryWidget->Selector->CurDisplayWidget->SetCanDeployColor();
	}
	else
	{
		InventoryWidget->Selector->CurDisplayWidget->SetCantDeployColor();
	}
}

void UInventoryComponent::MoveEquipmentSelector(FIntPoint Input)
{
	//�ӽ� ����� �¿�θ� �̵��ϵ��� ����
	if (Input.X == 0)
		return;
	int8 Next = (int8)GetSelectorStorage();
	//�Է� ������ ����
	Next += Input.X;


	//���� ���â���� �̵�
	InventoryWidget->Selector->SetStorage(static_cast<EItemStorageType>(Next));
	

	//�׸��� ��� ��ȸ��
	if (static_cast<EItemStorageType>(Next) == EItemStorageType::Max)//Max���� �����ϸ� �׸���� ���� ���� �̵�(���־�������)
	{
		InitSelector();
		//��� UI ��Ȱ��ȭ
		InventoryWidget->DisableSelectInfos();
		UE_LOG(LogTemp, Display, TEXT("Return to Grid"));
		return;
	}
	//�׸��� ��� ��ȸ��
	else if (static_cast<EItemStorageType>(Next) == EItemStorageType::Storage)//storage���� �����ϸ� �׸���� ���� ���� �̵�(���־�������)
	{
		InitSelector();
		//��� UI ��Ȱ��ȭ
		InventoryWidget->DisableSelectInfos();
		UE_LOG(LogTemp, Display, TEXT("Return to Grid"));
		return;
	}
}

void UInventoryComponent::DirectMoveSelector(EItemStorageType Storage)
{
	//�̵�
	InventoryWidget->Selector->SetInventoryIndex(SelectorPos);
	InventoryWidget->GetGrid(Storage)->InventoryGridPanel->
		AddChildToGrid(InventoryWidget->Selector, GetCoordinateByIndex(SelectorPos, Storage).Y,
			GetCoordinateByIndex(SelectorPos, Storage).X);
	if (InventoryWidget->Selector->CurDisplayWidget != nullptr)//���� ������ �������ִ� ������ ���÷��̰� ���� ���
	{
		if (!InventoryWidget->Selector->CurDisplayWidget)//������ �������� ��Ȳ
		{
			//������Ŭ�� ������ ��Ȳ
			InventoryWidget->ItemCanSelect();
		}

		SelectorVisible(ESlateVisibility::Hidden);

		InventoryWidget->Selector->CurDisplayWidget->ItemData->SetStoragePos(Storage);
		InventoryWidget->Selector->CurDisplayWidget->ItemData->SetItemPos(GetCoordinateByIndex(SelectorPos, Storage));

		InventoryWidget->Selector->CurDisplayWidget->SetInventoryIndex(SelectorPos);
		InventoryWidget->GetGrid(Storage)->InventoryGridPanel->
			AddChildToGrid(InventoryWidget->Selector->CurDisplayWidget, GetCoordinateByIndex(SelectorPos, Storage).Y,
				GetCoordinateByIndex(SelectorPos, Storage).X);
		//��ġ �������� ���� ����
		InventoryWidget->Selector->CurDisplayWidget->SetCanDeploy(
			IsItemAvailableForSlot(SelectorPos, InventoryWidget->Selector->CurItem->GetItemSize(), Storage));
	}
	//�߸��� ���� ������ ������
	else
	{
		SelectorPos = InventoryWidget->Selector->CurIndex;
	}
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
			temp->SetSelectorVisibility(true);
			result = Pos;
			//temp �������� ��ġ , ũ��
			ItemSpace->Key = temp->ItemData->GetItemPos();
			ItemSpace->Value = temp->ItemData->GetItemSize();
		}
		else if (InventoryWidget->Selector->CurDisplayWidget != temp)
		{
			temp->SetSelectorVisibility(false);
		}
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
	//���� �����۰� ��ġ�ϴ� ���� ����
	if (Pos == FIntPoint{ -1,-1 } && !InventoryWidget->Selector->CurDisplayWidget)
	{
		//���̱�
		SelectorVisible(ESlateVisibility::Visible);
		//�������� Ŀ������ ���� ���
		InventoryWidget->DisableSelectInfos();
	}
	//���� �����۰� ��ġ�ϴ� ���� �ִ� ���
	else
	{
		//������
		SelectorVisible(ESlateVisibility::Hidden);
		if (!InventoryWidget->Selector->CurDisplayWidget)
		{	//������Ŭ�� ������ ��Ȳ
			InventoryWidget->ItemCanSelect();//UI
		}
	}
}

void UInventoryComponent::SelectorVisible(ESlateVisibility Val)
{
	InventoryWidget->Selector->SetVisibility(Val);
}

void UInventoryComponent::MoveGrid(int8 IsInputRight)
{
	int8 IntStorage = static_cast<uint8>(GetSelectorStorage());
	IntStorage += IsInputRight;
	const int8 IntStorage_Max = InventoryWidget->StorageGrid->GetVisibility() == ESlateVisibility::Hidden ? 2 : 3;
	IntStorage = FMath::Clamp<uint8>(IntStorage, 1, IntStorage_Max);

	for (const auto& temp : GetItemContainer(GetSelectorStorage()))
	{
		temp->SetSelectorVisibility(false);
	}
	//���� ���� ��ġ ����
	InventoryWidget->Selector->SetStorage(static_cast<EItemStorageType>(IntStorage));
	//��ġ ����
	if (static_cast<EItemStorageType>(IntStorage) == EItemStorageType::BackPack)
	{
		SelectorPos = InventoryWidget->SecureContainerGrid->ColumnCount;
	}
	else
	{
		SelectorPos = 0;
	}
	MoveSelector(SelectorPos, true);
}

void UInventoryComponent::SelectItem(FVector2D Pos)
{
	//Ŀ���� �������� ���� ���� [��� ���� �ʿ� ����]
	if (InventoryWidget->Selector->GetVisibility() != ESlateVisibility::Hidden)
	{
		InventoryWidget->Selector->SetItemData(nullptr);
		return;
	}
	//���������� �������� �ִ� ���
	if (InventoryWidget->Selector->CurDisplayWidget != nullptr)
	{
		SelectMoveItem();
		return;
	}
	//���� ���� ������ ����Ű�� ������ ����
	SetCurItem();
	//�Ϲ� �׸����� ���
	if (!InventoryWidget->Selector->GetIsPointToEquipmentPlace())
	{
		SelectMoveItem();
	}
}

void UInventoryComponent::UseItemQuantityChange()
{
	//���� ���̱�
	CurUseItem->SetQuantity(CurUseItem->GetQuantity() - 1);
	if (CurUseItem->GetQuantity() > 0)
	{
		//�������� ���� ������Ʈ
		FindDisplayByItemData(CurUseItem)->UpdateQuantity();
	}
	//���� ������ ������ ����
	else
	{
		RemoveItem(CurUseItem);
	}
}

void UInventoryComponent::SelectMoveItem()
{
	//�̹� �����ؼ� �̵� ���̸�
	if (InventoryWidget->Selector->CurDisplayWidget)
	{
		//��ġ �����ϸ� ��ġ�ϰ� �ٲ� ���� ����
		if (InventoryWidget->Selector->CurDisplayWidget->GetCanDeploy())
		{
			//Ž��
			UInventoryItemDisplay* CurDisplay = FindDisplayByItemData(InventoryWidget->Selector->CurItem);
			if (!IsValid(CurDisplay))
				return;

			//����//���� display ȸ��
			if (GetCurItem()->GetIsRotate())
			{
				if (!CurDisplay->ItemData->GetIsRotate())
				{
					CurDisplay->IsRotate();
				}
			}
			else
			{
				if (CurDisplay->ItemData->GetIsRotate())
				{
					CurDisplay->IsRotate();
				}
			}

			if (CurDisplay->ItemData->GetStoragePos() == GetSelectorStorage())//���� �������� �ű涧
			{
				ItemMoveSameGrid(CurDisplay);
			}
			else//�ٸ� �������� �ű涧
			{
				//ItemMoveOtherGrid(CurDisplay);
			}

			InventoryWidget->Selector->Reset();
			CurItem = nullptr;
		}
		else
		{
			UE_LOG(LogTemp, Display, TEXT("cant deploy!"));
		}
	}
	else
	{
		SetCurItem();
		InventoryWidget->Selector->SetItemData(CurItem);
		//�ش�Ǵ� ���÷��� Ž��
		UInventoryItemDisplay* CurDisplay = FindDisplayByItemData(CurItem);
		CurDisplay->SetVisibility(ESlateVisibility::Hidden);
		if (CurDisplay->Owner)
		{
			ClearSlots(GetCoordinateByIndex(CurDisplay->GetInventoryIndex(), CurDisplay->Owner->GetStorageType()), CurItem->GetItemSize(), CurDisplay->Owner->GetStorageType());
		}

		//�ű�� ���� ������ ���� ������ ���÷��� ����
		UInventoryItemDisplay* DragVisual = CreateWidget<UInventoryItemDisplay>
			(GetWorld()->GetFirstPlayerController(), ItemDisplayClass);
		DragVisual->Owner = InventoryWidget->GetGrid(GetSelectorStorage());
		DragVisual->InvComp = this;
		DragVisual->Init(CurItem);
		DragVisual->SetIsFocus(true);
		//selector�� ����
		InventoryWidget->Selector->SetItemDisplay(DragVisual);
		DragVisual->SetSelectorVisibility(true);
		//������ ����
		InventoryWidget->GetGrid(GetSelectorStorage())->InventoryGridPanel->
			AddChildToGrid(InventoryWidget->Selector->CurDisplayWidget, GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).Y,
				GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).X);
	}
}

void UInventoryComponent::DeSelectItem()
{
	if (!InventoryWidget->Selector->CurDisplayWidget)
		return;
	//�ش�Ǵ� ���÷��� Ž��
	UInventoryItemDisplay* CurDisplay = FindDisplayByItemData(CurItem);
	if (!IsValid(CurDisplay))
		return;

	//���̵��� ����
	CurDisplay->SetVisibility(ESlateVisibility::Visible);
	//���� ������Ʈ
	CurDisplay->UpdateQuantity();
	CurDisplay->SetSelectorVisibility(false);
	FillSlots(CurDisplay->ItemData->GetItemPos(), CurDisplay->ItemData->GetItemSize(), CurItem->GetStoragePos());

	//�ʱ�ȭ
	InventoryWidget->Selector->Reset();
	//��ǥ �ʱ�ȭ
	SelectorPos = GetSlotIndexByCoordinate(CurItem->GetItemPos(), CurItem->GetStoragePos());
	//Ŀ�� �̵�
	SelectorVisible(ESlateVisibility::Hidden);
	MoveSelector(GetCoordinateByIndex(SelectorPos, GetSelectorStorage()), true);

	CurItem = nullptr;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent, Weight);
	DOREPLIFETIME_CONDITION(UInventoryComponent, IsOpenInventory, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(UInventoryComponent, TotalItemContainer, COND_OwnerOnly);
}

bool UInventoryComponent::PickUpItem(ABaseItem* Item)
{
	if (IsValid(Item->ItemData))
	{
		return PickUpItem(Item->ItemData);
	}
	return false;
}

bool UInventoryComponent::PickUpItem(UItemDataAsset* ItemData)
{
	UE_LOG(LogTemp, Display, TEXT("pick"));
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

void UInventoryComponent::SearchItem()
{
	////sphere Ʈ���̽��� �ֺ��� ������ Ȯ��
	//FVector Start = GetOwner()->GetActorLocation();
	//FVector End = GetOwner()->GetActorLocation();
	//TArray<AActor*> ActorsIgnore;//�����ϴ� ���� �迭
	//TArray<FHitResult> Hits; //�浹 ����� ������ �ִ� �迭
	////Ʈ���̽� ó��
	//const bool Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, SearchRange,
	//	UEngineTypes::ConvertToTraceType(ECC_Camera), false, ActorsIgnore, EDrawDebugTrace::ForDuration, Hits, true, FLinearColor::Gray, FLinearColor::Blue, 60.0f);
	//if (Hit)//�浹�� ���� ������
	//{
	//	for (const FHitResult HitResult : Hits)
	//	{
	//		if (Cast<ABaseItem>(HitResult.GetActor()))
	//		{
	//			ABaseItem* Item = Cast<ABaseItem>(HitResult.GetActor());
	//			//�ߺ��� �ѱ�
	//			if (FieldDropItems.Find(Item->GetUniqueID()) != nullptr)
	//				continue;;
	//			//UID�� ���ؼ� TMap�� ����
	//			FieldDropItems.Add({ Item->GetUniqueID() , Item });
	//			//�������� ���¸� ���޹ޱ� ���ؼ� �����ۿ� ���¸� ���޹޴� �����ڷ� ���
	//			Item->AddObserver(Cast<ACharacter>(GetOwner()));
	//			//������ �߰�
	//			Item->ItemData->SetStoragePos(EItemStorageType::Storage);
	//			AddItem(Item->ItemData, EItemStorageType::Storage);
	//		}
	//	}
	//}
	//
	//if (!FieldDropItems.IsEmpty())//������� ������ Ȱ��ȭ
	//	InventoryWidget->StorageGrid->SetVisibility(ESlateVisibility::Visible);
	//else//��������� ��Ȱ��ȭ
	//	InventoryWidget->StorageGrid->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryComponent::ClearSearchItem()
{
	//for (auto& temp : FieldDropItems)
	//{
	//	//���� ���
	//	temp.Value->DeleteObserver(Cast<ACharacter>(GetOwner()));
	//	//������ ���÷��� ����
	//	RemoveItem(temp.Value->ItemData);
	//}
	////TMap �ʱ�ȭ
	//FieldDropItems.Empty();
	////���� ����
	////InventoryWidget->StorageGrid->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryComponent::ItemMoveOtherGrid(UInventoryItemDisplay* CurDisplay)
{
	//���ο� �׸��忡 ������ �߰�
	AddItem(GetCurItem(), SelectorPos, GetSelectorStorage());
	//���� �׸��忡�� ������ ������ ����
	RemoveItem(CurDisplay->ItemData);
	//�κ��丮 ���÷��� ����
	CurDisplay->SetInventoryIndex(SelectorPos);
	FillSlots(GetCoordinateByIndex(SelectorPos, GetSelectorStorage()), GetCurItem()->GetItemSize(), GetSelectorStorage());
	MoveSelector(GetCoordinateByIndex(SelectorPos, GetSelectorStorage()), true);
}

void UInventoryComponent::ItemMoveSameGrid(UInventoryItemDisplay* CurDisplay)
{
	CurDisplay->SetVisibility(ESlateVisibility::Visible);
	//���� display ȸ��
	if (GetCurItem()->GetIsRotate())
	{
		if (!CurDisplay->ItemData->GetIsRotate())
		{
			CurDisplay->IsRotate();
		}
	}
	else
	{
		if (CurDisplay->ItemData->GetIsRotate())
		{
			CurDisplay->IsRotate();
		}
	}
	//�׸��忡 ����
	if (UGridSlot* GridSlot = Cast<UGridSlot>(CurDisplay->Slot))
	{
		GridSlot->SetColumn(GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).X);
		GridSlot->SetRow(GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).Y);
	}
	CurDisplay->SetInventoryIndex(SelectorPos);
	CurDisplay->ItemData->SetItemPos(GetCoordinateByIndex(SelectorPos, GetSelectorStorage()));
	FillSlots(GetCoordinateByIndex(SelectorPos, GetSelectorStorage()), GetCurItem()->GetItemSize(), GetSelectorStorage());
	MoveSelector(GetCoordinateByIndex(SelectorPos, GetSelectorStorage()), true);
}

void UInventoryComponent::RemoveItem(UItemDataAsset* ItemData)
{
	if (InventoryWidget)
	{
		if (ItemData->GetStoragePos() == GetSelectorStorage())
		{
			MoveSelector(ItemData->GetItemPos(), true);
		}
		GridRemoveItem(ItemData, ItemData->GetStoragePos());
	}
}

void UInventoryComponent::GridRemoveItem(UItemDataAsset* ItemData, EItemStorageType StorageType)
{
	//�ش�Ǵ� ���÷��� Ž��
	UInventoryItemDisplay* DeleteDisplay = FindDisplayByItemData(ItemData);

	if (!IsValid(DeleteDisplay))
		return;

	if (DeleteDisplay)
	{
			DeleteDisplay->SetVisibility(ESlateVisibility::Hidden);
			DeleteDisplay->RemoveFromParent();
			ClearSlots(DeleteDisplay->ItemData->GetItemPos(), DeleteDisplay->GetItemSize(), DeleteDisplay->ItemData->GetStoragePos());
			GetItemContainer(StorageType).Remove(DeleteDisplay);
	}
}

void UInventoryComponent::RemoveEquipSlotOrGrid(UInventoryItemDisplay* InventoryItemDisplay)
{
	//���� �׸��忡�� ������ ������ ����
	RemoveItem(InventoryItemDisplay->ItemData);
}

void UInventoryComponent::ResetInventory()
{
	//�׸��� �ʱ�ȭ
	InventoryWidget->SecureContainerGrid->InitInventoryWidget();
	InventoryWidget->BackpackGrid->InitInventoryWidget();
	InventoryWidget->StorageGrid->InitInventoryWidget();
	//StorageGrid ������
	InventoryWidget->StorageGrid->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryComponent::ResetInventory(EItemStorageType Storage)
{
	switch (Storage)
	{
	case EItemStorageType::Storage:
		//�׸��� �ʱ�ȭ
		InventoryWidget->StorageGrid->InitInventoryWidget();
		GetItemContainer(EItemStorageType::Storage).Empty();
		//StorageGrid ������
		InventoryWidget->StorageGrid->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EItemStorageType::BackPack:
		//�׸��� �ʱ�ȭ
		InventoryWidget->BackpackGrid->InitInventoryWidget();
		break;
	case EItemStorageType::SecureContainer:
		//�׸��� �ʱ�ȭ
		InventoryWidget->SecureContainerGrid->InitInventoryWidget();
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("not define Storage Area"));
		break;
	}

}

void UInventoryComponent::DropItem(FName ID, int32 Quantity, UItemDataAsset* Item)
{
	if (GetWorld()->GetFirstPlayerController() && IsValid(Item))
	{
		FItemNetInfo TempItemNetInfo;
		TempItemNetInfo.ID = ID;
		TempItemNetInfo.Quantity = Quantity;
		DropItem_Server(TempItemNetInfo);
		//������ ���÷��� �ʱ�ȭ
		if (InventoryWidget->Selector->GetIsPointToEquipmentPlace())
		{
			//switch (GetSelectorStorage())
			//{
			//case EItemStorageType::PrimaryWeapon:
			//	//������� ������ ���� ����
			//	if (!InventoryWidget->PrimaryWeapon->GetIsSlotEmpty())
			//	{
			//		InventoryWidget->PrimaryWeapon->RemoveEquipment();
			//	}
			//	break;
			//case EItemStorageType::PrimaryWeapon1:
			//	//������� ������ ���� ����
			//	if (!InventoryWidget->PrimaryWeapon1->GetIsSlotEmpty())
			//	{
			//		InventoryWidget->PrimaryWeapon1->RemoveEquipment();
			//	}
			//	break;
			//default:
			//	break;
			//}
		}
		else
		{
			InitSelector();
			//����
			RemoveItem(Item);
		}

		//��� UI ��Ȱ��ȭ
		InventoryWidget->DisableSelectInfos();
	}
}

void UInventoryComponent::DropItem_Server_Implementation(const FItemNetInfo& ItemNetInfo)
{
	//������ ����
	FActorSpawnParameters SpawnParams;
	FVector SpawnPosOffset = FVector::ZeroVector;
	SpawnPosOffset = GetOwner()->GetActorRotation().Vector();
	SpawnPosOffset = SpawnPosOffset.GetSafeNormal();
	SpawnPosOffset *= 100;
	ABaseItem* SpawnItem = GetWorld()->SpawnActor<ABaseItem>(ItemBlueprint,GetOwner()->GetActorLocation() + SpawnPosOffset, FRotator::ZeroRotator, SpawnParams);
	SpawnItem->SetItemData(ItemNetInfo);
	
	DropItem_NetMultiCast(ItemNetInfo);
}

void UInventoryComponent::DropItem_NetMultiCast_Implementation(const FItemNetInfo& ItemNetInfo)
{
	//FItem NewItem = *Cast<UAdvancedFriendsGameInstance>(GetWorld()->GetGameInstance())->GetItemInfo(FName(FString::FromInt(ItemNetInfo.ID)));
	//Cast<AZCCharacter>(GetOwner())->SetWeight(-NewItem.Weight, ItemNetInfo.Quantity);
	////�������� ���� ���
	//DropItemEvent();
}

void UInventoryComponent::DropItemAll()
{
	//��� ������ ������ ����

	if (Cast<ACharacter>(GetOwner())->IsLocallyControlled())
	{
		for (EItemStorageType Val : TEnumRange<EItemStorageType>())
		{
			if (Val == EItemStorageType::SecureContainer)//���� �����̳� ���� 
				continue;
			if (Val == EItemStorageType::BackPack && ItemContainer[Val].Num() == 1)
				continue;

			if (ItemContainer[Val].IsEmpty())
				continue;
			else
			{
				for (auto temp : ItemContainer[Val])
				{
					TotalItemContainer.Add(temp->ItemData);
				}
			}
		}

		/*�κ��丮 ����ִ��� Ȯ��*/
		//UpdateTotalItemContainer();
		if (TotalItemContainer.IsEmpty())
			return;

		//������ ���� array ����
		TArray<FItemNetInfo> ItemNetInfos;
		for (auto& temp : TotalItemContainer)
		{
			FItemNetInfo TempItemNetInfo;

			TempItemNetInfo.ID = temp->GetID();
			TempItemNetInfo.Quantity = temp->GetQuantity();
			ItemNetInfos.Add(TempItemNetInfo);
		}

		//������ ����
		TotalItemContainer.Empty();
		for (EItemStorageType Val : TEnumRange<EItemStorageType>())
		{
			if (Val == EItemStorageType::SecureContainer)
				continue;
			else
			{
				for (auto temp : ItemContainer[Val])
				{
					temp->SetVisibility(ESlateVisibility::Hidden);
				}
				ItemContainer[Val].Empty();
			}
		}
		//��ȣ ���������ŭ ������ ���� ��������� ó���ϱ�
		InventoryWidget->BackpackGrid->InitInventoryWidget();
		//����&���â �ʱ�ȭ
		//InventoryWidget->PrimaryWeapon->RemoveEquipment();
		//InventoryWidget->PrimaryWeapon1->RemoveEquipment();

		//�迭 ������
		DropItemAll_Server(ItemNetInfos);
	}
}

void UInventoryComponent::UpdateTotalItemContainer()
{
	//UE_LOG(LogTemp, Display, TEXT("%s"), *GetWorld()->GetFirstPlayerController()->GetName());
	//for (auto Temp : TotalItemContainer)
	//{
	//	UE_LOG(LogTemp, Display, TEXT("%s"), *Temp->GetItemName().ToString());
	//}
}

void UInventoryComponent::DropItemAll_Server_Implementation(const TArray<FItemNetInfo>& Items)
{
	//FActorSpawnParameters SpawnParams;
	//SpawnParams.Owner = this->GetOwner();
	//AZCBodyBag* SpawnBodyBag = GetWorld()->SpawnActor<AZCBodyBag>(BodyBagBlueprint, GetOwner()->GetActorLocation(), FRotator::ZeroRotator, SpawnParams);
	//if (!Items.IsEmpty())
	//{
	//	SpawnBodyBag->AddItem(Items);
	//}
}


bool UInventoryComponent::AddItem(UItemDataAsset* ItemDataAsset, EItemStorageType StorageType)
{
	const int Index = GetFirstAvailableSlotIndex(ItemDataAsset->GetItemSize(), StorageType);

	if (ItemDataAsset->IsStackable())//��ġ�� ������ ���
	{
		for (const auto ItemRef : GetItemContainer(StorageType))
		{
			ItemRef->InvComp = this;
			if (ItemRef->ItemData->GetItemName().EqualTo(ItemDataAsset->GetItemName()))
			{
				//�ִ� ������ �Ѿ�� ��ġ�� �Ұ��� ����
				if (ItemRef->ItemData->GetMaxQuantity() >= (ItemDataAsset->GetQuantity() + ItemRef->ItemData->GetQuantity()))
				{
					ItemRef->ItemData->AddQuantity(ItemDataAsset->GetQuantity());
					ItemRef->UpdateQuantity();
					return true;
				}
			}
		}
	}

	if (Index != -1)
	{
		UInventoryItemDisplay* InventoryItemDisplay = CreateWidget<UInventoryItemDisplay>(InventoryWidget, ItemDisplayClass);
		InventoryItemDisplay->Owner = InventoryWidget->GetGrid(StorageType);
		InventoryItemDisplay->Init(ItemDataAsset);
		InventoryItemDisplay->SetInventoryIndex(Index);
		InventoryItemDisplay->InvComp = this;
		const FIntPoint ItemStartPoint = GetCoordinateByIndex(Index, StorageType);

		InventoryWidget->GetGrid(StorageType)->InventoryGridPanel->AddChildToGrid(InventoryItemDisplay, ItemStartPoint.Y, ItemStartPoint.X);
		UItemDataAsset* CurDataAsset = InventoryItemDisplay->ItemData;
		CurDataAsset->SetItemPos(ItemStartPoint);
		CurDataAsset->SetStoragePos(StorageType);
		InventoryItemDisplay->ItemData = CurDataAsset;
		InventoryItemDisplay->UpdateQuantity();
		GetItemContainer(StorageType).Add(InventoryItemDisplay);
		FillSlots(ItemStartPoint, ItemDataAsset->GetItemSize(), StorageType);
		return true;
	}

	return false;
}

bool UInventoryComponent::AddItem(UItemDataAsset* ItemDataAsset, int SlotIndex, EItemStorageType StorageType)
{
	UInventoryItemDisplay* InventoryItemDisplay = CreateWidget<UInventoryItemDisplay>(InventoryWidget, ItemDisplayClass);
	InventoryItemDisplay->Init(ItemDataAsset);
	InventoryItemDisplay->SetInventoryIndex(SlotIndex);
	InventoryItemDisplay->InvComp = this;
	InventoryItemDisplay->Owner = InventoryWidget->GetGrid(StorageType);
	InventoryItemDisplay->ItemData->SetStoragePos(StorageType);
	InventoryItemDisplay->Selector->SetVisibility(ESlateVisibility::Hidden);
	InventoryItemDisplay->UpdateQuantity();
	const FIntPoint ItemStartPoint = GetCoordinateByIndex(SlotIndex, StorageType);

	InventoryWidget->GetGrid(StorageType)->InventoryGridPanel->AddChildToGrid(InventoryItemDisplay, ItemStartPoint.Y, ItemStartPoint.X);
	InventoryItemDisplay->ItemData->SetItemPos(ItemStartPoint);
	GetItemContainer(StorageType).Add(InventoryItemDisplay);
	FillSlots(ItemStartPoint, ItemDataAsset->GetItemSize(), StorageType);

	//�ʵ� ��� �������� �������� ��� �ʵ忡�� �����������
	//ABaseItem* FieldItem =
	
	return true;
}

bool UInventoryComponent::IsItemAvailableForSlot(const int Index, const FIntPoint& ItemSize,
	EItemStorageType StorageType) const
{
	const auto Coordinate = GetCoordinateByIndex(Index, StorageType);

	const int ItemColumnSize = ItemSize.Y;
	const int ItemRowSize = ItemSize.X;

	//������ȿ� �����ִ��� Ȯ��
	for (int i = 0; i < ItemRowSize; i++)
	{
		for (int j = 0; j < ItemColumnSize; j++)
		{
			if (ItemSlotMap[StorageType]
				[ItemSlots[StorageType]
				[GetSlotIndexByCoordinate((Coordinate.X + i) % InventoryWidget->GetGrid(StorageType)->ColumnCount, (Coordinate.Y + j) % InventoryWidget->GetGrid(StorageType)->RowCount, StorageType)]])
			{
				return false;
			}
		}
	}

	if ((Coordinate.X + ItemSize.X > InventoryWidget->GetGrid(StorageType)->ColumnCount) || (Coordinate.Y + ItemSize.Y > InventoryWidget->GetGrid(StorageType)->RowCount))
	{
		return false;
	}
	return true;
}

int UInventoryComponent::GetSlotIndexByCoordinate(const int Column, const int Row, EItemStorageType StorageType) const
{
	if (Column > InventoryWidget->GetGrid(StorageType)->ColumnCount || Row > InventoryWidget->GetGrid(StorageType)->RowCount)
	{
		return -1;
	}

	return (Column + (Row * InventoryWidget->GetGrid(StorageType)->ColumnCount));
}

int UInventoryComponent::GetSlotIndexByCoordinate(const FIntPoint Pos, EItemStorageType StorageType) const
{
	if (Pos.Y > InventoryWidget->GetGrid(StorageType)->ColumnCount || Pos.X > InventoryWidget->GetGrid(StorageType)->RowCount)
	{
		return -1;
	}

	return (Pos.X + (Pos.Y * InventoryWidget->GetGrid(StorageType)->ColumnCount));
}

FIntPoint UInventoryComponent::GetCoordinateByIndex(const int Index, EItemStorageType StorageType) const
{
	if (Index >= 0 && Index < ItemSlots[StorageType].Num())
	{
		if (UInventorySlot* InvSlot = Cast<UInventorySlot>(ItemSlots[StorageType][Index]))
		{
			if (UGridSlot* GridSlot = Cast<UGridSlot>(InvSlot->Slot))
			{
				return FIntPoint{ GridSlot->Column, GridSlot->Row };
			}
		}
	}
	return FIntPoint{ -1,-1 };
}

UInventoryItemDisplay* UInventoryComponent::FindDisplayByItemData(UItemDataAsset* Data)
{
	for (auto temp : ItemContainer[Data->StoragePosition])
	{
		//ã�� ���
		if (temp->ItemData == Data)
			return temp;
		//ã�� ���
		//if (temp->ItemData->GetNetPushId() == Data->GetNetPushId())
		//	return temp;
	}
	//�� ã�� ���
	return nullptr;
}

void UInventoryComponent::SortItems(EItemStorageType StorageType)
{
	ClearSlots({ 0,0 }, { InventoryWidget->GetGrid(StorageType)->RowCount, InventoryWidget->GetGrid(StorageType)->ColumnCount }, StorageType);

	GetItemContainer(StorageType).Sort([=](const auto& Left, const auto& Right)
		{
			return Left.GetTotalSlotForItem() > Right.GetTotalSlotForItem();
		});

	for (const auto& Item : GetItemContainer(StorageType))
	{
		const int Index = GetFirstAvailableSlotIndex(Item->ItemData->GetItemSize(), StorageType);
		const FIntPoint ItemStartPoint = GetCoordinateByIndex(Index, StorageType);

		Item->SetInventoryIndex(Index);

		InventoryWidget->GetGrid(StorageType)->InventoryGridPanel->AddChildToGrid(Item, ItemStartPoint.Y, ItemStartPoint.X);

		FillSlots(ItemStartPoint, Item->GetItemSize(), StorageType);
	}
}

void UInventoryComponent::FillSlots(const FIntPoint& StartPoint, const FIntPoint& ItemSize,
	EItemStorageType StorageType)
{
	for (int i = 0; i < ItemSize.X; i++)
	{
		for (int j = 0; j < ItemSize.Y; j++)
		{
			auto SingleSlot = GetItemSlots(StorageType)
				[
					GetSlotIndexByCoordinate((StartPoint.X + i) % InventoryWidget->GetGrid(StorageType)->ColumnCount, (StartPoint.Y + j) % InventoryWidget->GetGrid(StorageType)->RowCount, StorageType)
				];
			GetItemSlotmMap(StorageType).Add(SingleSlot, true);
		}
	}
}

void UInventoryComponent::ClearSlots(const FIntPoint& StartPoint, const FIntPoint& ItemSize,
	EItemStorageType StorageType)
{
	UE_LOG(LogTemp,Display,TEXT("%d %d %d %d"),StartPoint.X,StartPoint.Y, ItemSize.X, ItemSize.Y);
	for (int i = 0; i < ItemSize.X; i++)
	{
		for (int j = 0; j < ItemSize.Y; j++)
		{
			UInventorySlot* SingleSlot = GetItemSlots(StorageType)
				[
					GetSlotIndexByCoordinate((StartPoint.X + i) % InventoryWidget->GetGrid(StorageType)->ColumnCount, (StartPoint.Y + j) % InventoryWidget->GetGrid(StorageType)->RowCount, StorageType)
				];
			GetItemSlotmMap(StorageType).Add(SingleSlot, false);
		}
	}
}

int UInventoryComponent::GetFirstAvailableSlotIndex(const FIntPoint& ItemSize, EItemStorageType StorageType) const
{
	int i = 0;

	for (const auto& SingleSlot : ItemSlotMap[StorageType])
	{
		if (IsItemAvailableForSlot(i, ItemSize, StorageType))
		{
			return i;
		}

		i++;
	}

	return -1;
}

void UInventoryComponent::SelectorItemDisplayRoatate()
{
	InventoryWidget->Selector->CurDisplayWidget->IsRotate();
}

void UInventoryComponent::SetIsOpenInventory(bool IsOpen)
{
	if (IsOpen)
	{
		//�ݱ� BP�̺�Ʈ
		OpenCloseEvent(IsOpen);
		//��ó ������ Ž��
		SearchItem();
	}
	else
	{
		//��ó ������ Ž�� ���
		ClearSearchItem();
		for (const auto& temp : GetItemContainer(GetSelectorStorage()))
		{
			temp->SetSelectorVisibility(false);
		}
	}
	//InitSelector();
	SetIsOpenInventory_Server(IsOpen);
}

void UInventoryComponent::SetIsOpenInventory_Server_Implementation(bool IsOpen)
{
	SetIsOpenInventory_NetMulticast(IsOpen);
}


void UInventoryComponent::SetIsOpenInventory_NetMulticast_Implementation(bool IsOpen)
{
	IsOpenInventory = IsOpen;

	////���� ������
	//if (!IsOpen && CurBodyBag != nullptr)
	//{
	//	//������ ������ ����Ʈ �ʱ�ȭ
	//	CurBodyBag->Reset_Server();
	//	if (!GetItemContainer(EItemStorageType::Storage).IsEmpty())//������� ���� ���/* �ʵ� ��� ������ ������ �� �κ� �����ؾ���
	//	{
	//		//������ ���� array ����
	//		TArray<FItemNetInfo> ItemNetInfos;
	//		for (auto& temp : GetItemContainer(EItemStorageType::Storage))
	//		{
	//			FItemNetInfo TempItemNetInfo;
	//
	//			TempItemNetInfo.ID = temp->ItemData->GetId();
	//			TempItemNetInfo.Quantity = temp->ItemData->GetQuantity();
	//			ItemNetInfos.Add(TempItemNetInfo);
	//
	//			temp->RemoveFromParent();
	//		}
	//		CurBodyBag->AddItem(ItemNetInfos);
	//	}
	//	//������ �ʱ�ȭ
	//	ResetInventory(EItemStorageType::Storage);
	//	CurBodyBag = nullptr;
	//}
	//
}

void UInventoryComponent::SetIsUsingMed(bool val)
{
	IsUsingMed = val;
}

bool UInventoryComponent::GetIsUsingMed()
{
	return IsUsingMed;
}

void UInventoryComponent::SetCurItem(UItemDataAsset* cur)
{
	CurItem = nullptr;
	if (cur != nullptr)
	{
		CurItem = cur;
		return;
	}
	//�Է� ���� ȣ�⸸ �ϴ� ���
	if (InventoryWidget->Selector->GetVisibility() == ESlateVisibility::Hidden)//���� ������ ����Ű�� ���� �ִ��� ������ Ȯ��
	{
		for (const auto& temp : GetItemContainer(GetSelectorStorage()))
		{
			//temp �������� ��ġ , ũ��
			if ((temp->ItemData->GetItemPos().X <= GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).X && GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).X < temp->ItemData->GetItemPos().X + temp->ItemData->GetItemSize().X)
				&& (temp->ItemData->GetItemPos().Y <= GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).Y && GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).Y < temp->ItemData->GetItemPos().Y + temp->ItemData->GetItemSize().Y))
			{//�����۰� ������ ��ĥ ���  �������� ������ ��ġ�� ��ȯ
				CurItem = temp->ItemData;
				return;
			}
		}
	}
}

FIntPoint UInventoryComponent::GetGridSize(EItemStorageType Storage)
{
	return { InventoryWidget->GetGrid(Storage)->ColumnCount,InventoryWidget->GetGrid(Storage)->RowCount };
}

EItemStorageType UInventoryComponent::GetSelectorStorage()
{
	return InventoryWidget->Selector->StorageType;
}

void UInventoryComponent::IA_InventoryMoveUp()
{
	if (GetIsOpenInventory())
	{
		MoveSelector({ 0 , -1 });
	}
}

void UInventoryComponent::IA_InventoryMoveDown()
{
	if (GetIsOpenInventory())
	{
		MoveSelector({ 0,1 });
	}
}

void UInventoryComponent::IA_InventoryMoveRight()
{
	if (GetIsOpenInventory())
	{
		MoveSelector({ 1 ,0 });
	}
}

void UInventoryComponent::IA_InventoryMoveLeft()
{
	if (GetIsOpenInventory())
	{
		MoveSelector({ -1 , 0 });
	}
}

void UInventoryComponent::IA_InventoryGridChange(float value)
{
	if (GetIsOpenInventory())
	{
		MoveGrid(value);
	}
}

void UInventoryComponent::IA_InventoryItemSelect()
{
	if (GetIsOpenInventory())
	{
		SelectItem();
	}
}

void UInventoryComponent::IA_InventoryItemDeSelectOrClose()
{
	if (GetIsOpenInventory())
	{
		if (IsValid(GetCurItem()))
		{
			DeSelectItem();
		}
		else
		{
			//���� �ʿ�
			//APlayerController* Controller = GetWorld()->GetFirstPlayerController();
			//if (Controller != nullptr)
			//	Controller->ShowInventory(false);
		}
	}
}

void UInventoryComponent::IA_InventoryItemRotate()
{
	if (GetIsOpenInventory())
	{
		if (GetCurItem())
		{
			SelectorItemDisplayRoatate();
		}
	}
}

void UInventoryComponent::IA_InventoryDrop()
{
	if (IsValid(InventoryWidget->Selector->CurItem))
	{
		DropItem(GetCurItem()->GetID(), GetCurItem()->Quantity, GetCurItem());
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
