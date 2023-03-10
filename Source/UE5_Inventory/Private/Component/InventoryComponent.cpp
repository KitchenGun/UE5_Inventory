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

//enum 순회용
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
	/*인벤토리 아이템 선택기 초기화*/
	//컴포넌트 변수
	SelectorPos = 0;
	CurItem = nullptr;
	//아이템 선택 위젯
	InventoryWidget->Selector->Reset();
	InventoryWidget->Selector->SetStorage(EItemStorageType::BackPack);
	InventoryWidget->Selector->SetInventoryIndex(0);
	const FIntPoint ItemStartPoint = GetCoordinateByIndex(0, EItemStorageType::BackPack);
	InventoryWidget->GetGrid(EItemStorageType::SecureContainer)->InventoryGridPanel->
		AddChildToGrid(InventoryWidget->Selector, ItemStartPoint.Y, ItemStartPoint.X);
	//size 초기화
	int32 TileSize = InventoryWidget->BackpackGrid->TileSize;
	//시작 공간에 아이템이 없을 경우
	InventoryWidget->Selector->SetSize(TileSize);
	//아이템이 있을 경우
	for (auto& temp : GetItemContainer(EItemStorageType::SecureContainer))
	{
		if (GetSlotIndexByCoordinate(temp->ItemData->GetItemPos(), EItemStorageType::SecureContainer) == 0)
		{
			InventoryWidget->Selector->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	//장비창 가리키고 있던 경우 취소
	InventoryWidget->Selector->SetIsPointToEquipmentPlace(false);

	//선택 위젯 보이게 하기
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

	//가방에 속하는 경우 보안 컨테이너 영역에 들어가지 않도록 예외처리
	if (Storage == EItemStorageType::BackPack)
	{
		Min_X = GetGridSize(EItemStorageType::SecureContainer).X;
		Min_Y = GetGridSize(EItemStorageType::SecureContainer).Y;
	}
	//범위 확인
	if ((Min_X <= Pos.X && Pos.X < Max_X) && (Min_Y <= Pos.Y && Pos.Y < Max_Y))
	{
		return true;
	}

	//기존 그리드에서는 범위 밖으로 나갔지만 다른 그리드에 들어가는 경우(백팩)
	if (Storage == EItemStorageType::BackPack)
	{
		if (0 <= Pos.X && Pos.X < Max_X && 0 <= Pos.Y && Pos.Y < Max_Y)//범위 안
		{
			return true;
		}
	}
	//기존 그리드에서는 범위 밖으로 나갔지만 다른 그리드에 들어가는 경우(보안상자)
	else if (Storage == EItemStorageType::SecureContainer)
	{
		if (Max_X <= Pos.X || Max_Y <= Pos.Y)
		{
			if (Pos.X < GetGridSize(EItemStorageType::BackPack).X || Pos.Y < GetGridSize(EItemStorageType::BackPack).Y)
			{
				MoveGrid(1);
				//move selector 함수에서 사용하기위해서 추가적으로 수정
				Storage = EItemStorageType::BackPack;
				return true;
			}
		}
	}

	/*중요*/
	//그리드에 벗어나서 선택 위젯의 위치를 옮기는 기능

	//x축이 아닌 Y축의 경우 해당사항이 없음으로 반환
	if (Min_X <= Pos.X && Pos.X < Max_X)
		return false;

	////백팩 & 보안 컨테이너
	//if (GetSelectorStorage() == EItemStorageType::SecureContainer || GetSelectorStorage() == EItemStorageType::BackPack)
	//{
	//	//장비창 방향으로 움직인 경우
	//	if (Min_X > Pos.X)
	//	{
	//		//장비창 가리킨다고 변수 수정
	//		InventoryWidget->Selector->SetIsPointToEquipmentPlace(true);
	//		//가리키는 공간 변경
	//		InventoryWidget->Selector->SetStorage(EItemStorageType::PrimaryWeapon);
	//		InventoryWidget->PrimaryWeapon->WeaponDisplayWidget->SetVisibility(ESlateVisibility::Visible);
	//		InventoryWidget->PrimaryWeapon->WeaponDisplayWidget->SetSelectorVisibility(true);
	//		if (!InventoryWidget->PrimaryWeapon->GetIsSlotEmpty())
	//			InventoryWidget->ItemCanSelect();
	//
	//		//보이지 않도록 변경
	//		InventoryWidget->Selector->SetVisibility(ESlateVisibility::Hidden);
	//	}
	//	else if (Max_X <= Pos.X)
	//	{
	//		//가리키는 공간 변경
	//		//Storage = EItemStorageType::Storage;
	//		MoveGrid(1);
	//	}
	//
	//}
	////그외의 장비 구역
	//else if (GetSelectorStorage() == EItemStorageType::Storage)
	//{
	//	//장비창 방향으로 움직인 경우
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
	//size 구하기
	const int32 TileSize = InventoryWidget->BackpackGrid->TileSize;
	//temp 아이템의 위치 , 크기 저장할 변수
	TPair<FIntPoint, FIntPoint>SelectorItemInfo{ FIntPoint{ 0,0 }, FIntPoint{ 0,0 } };
	TPair<FIntPoint, FIntPoint>PrevSelectorItemInfo{ FIntPoint{ 0,0 }, FIntPoint{ 0,0 } };
	TPair<FIntPoint, FIntPoint>* ItemSpace = &SelectorItemInfo;
	TPair<FIntPoint, FIntPoint>* PrevItemSpace = &PrevSelectorItemInfo;
	//그리드 공간이 아니면 따로 처리하고 반환
	if (InventoryWidget->Selector->GetIsPointToEquipmentPlace())
	{
		MoveEquipmentSelector(Pos);
		return;
	}
	/*그리드 상 이동 처리*/
	//직접적인 인덱스 전달&이동해야하는 값 입력
	if (IsDirect)
	{
		//범위 밖일 경우
		if (!IsOverGrid(Pos, Storage))
			return;
		//탐색
		Pos = SelectorFindItemPos(Pos, ItemSpace, Storage);
		//좌표 저장
		SelectorPos = GetSlotIndexByCoordinate(Pos.X, Pos.Y, Storage);
		//이동
		DirectMoveSelector(Storage);

		//UI 업데이트
		SelectorUIUpdate(Pos);
		SelectorCurItemDisplayUpdate();
	}
	//이동 인풋을 통해서 이동하는 경우
	else
	{
		//커서 이동 방향
		FIntPoint Dir = Pos;
		//커서와 아이템 충돌 여부 확인
		//FIntPoint TempPrevSelectorPos = SelectorFindItemPos(GetCoordinateByIndex(SelectorPos, Storage), PrevItemSpace, Storage);
		FIntPoint TempSelectorPos = SelectorFindItemPos(GetCoordinateByIndex(SelectorPos, Storage), ItemSpace, Storage);

		//아이템의 범위안에 커서가 있는 경우 이동값을 수정
		if (InventoryWidget->Selector->GetVisibility() == ESlateVisibility::Hidden && InventoryWidget->Selector->CurDisplayWidget == nullptr)
		{
			if (Dir.X == 0)//상하 이동
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
			else if (Dir.Y == 0)//좌우 이동
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
		//들어온값과 기존위치 더하기
		Pos = GetCoordinateByIndex(SelectorPos, Storage) + Pos;
		//범위 확인
		if (!IsOverGrid(Pos, Storage))
			return;
		//좌표 저장
		SelectorPos = GetSlotIndexByCoordinate(Pos, Storage);
		//이동
		DirectMoveSelector(Storage);
		//UI 업데이트
		SelectorUIUpdate(SelectorFindItemPos(GetCoordinateByIndex(SelectorPos, Storage), ItemSpace, Storage));
		SelectorCurItemDisplayUpdate();
	}
}

void UInventoryComponent::SelectorCurItemDisplayUpdate()
{
	//선택 위젯의 아이템이 들려있지 않으면 반환처리
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
	//임시 현재는 좌우로만 이동하도록 제작
	if (Input.X == 0)
		return;
	int8 Next = (int8)GetSelectorStorage();
	//입력 방향을 더함
	Next += Input.X;


	//다음 장비창으로 이동
	InventoryWidget->Selector->SetStorage(static_cast<EItemStorageType>(Next));
	

	//그리드 모두 순회시
	if (static_cast<EItemStorageType>(Next) == EItemStorageType::Max)//Max까지 도달하면 그리드로 선택 위젯 이동(비주얼적으로)
	{
		InitSelector();
		//기능 UI 비활성화
		InventoryWidget->DisableSelectInfos();
		UE_LOG(LogTemp, Display, TEXT("Return to Grid"));
		return;
	}
	//그리드 모두 순회시
	else if (static_cast<EItemStorageType>(Next) == EItemStorageType::Storage)//storage까지 도달하면 그리드로 선택 위젯 이동(비주얼적으로)
	{
		InitSelector();
		//기능 UI 비활성화
		InventoryWidget->DisableSelectInfos();
		UE_LOG(LogTemp, Display, TEXT("Return to Grid"));
		return;
	}
}

void UInventoryComponent::DirectMoveSelector(EItemStorageType Storage)
{
	//이동
	InventoryWidget->Selector->SetInventoryIndex(SelectorPos);
	InventoryWidget->GetGrid(Storage)->InventoryGridPanel->
		AddChildToGrid(InventoryWidget->Selector, GetCoordinateByIndex(SelectorPos, Storage).Y,
			GetCoordinateByIndex(SelectorPos, Storage).X);
	if (InventoryWidget->Selector->CurDisplayWidget != nullptr)//선택 위젯에 가지고있는 아이템 디스플레이가 없을 경우
	{
		if (!InventoryWidget->Selector->CurDisplayWidget)//이전에 보여졌던 상황
		{
			//아이템클릭 가능한 상황
			InventoryWidget->ItemCanSelect();
		}

		SelectorVisible(ESlateVisibility::Hidden);

		InventoryWidget->Selector->CurDisplayWidget->ItemData->SetStoragePos(Storage);
		InventoryWidget->Selector->CurDisplayWidget->ItemData->SetItemPos(GetCoordinateByIndex(SelectorPos, Storage));

		InventoryWidget->Selector->CurDisplayWidget->SetInventoryIndex(SelectorPos);
		InventoryWidget->GetGrid(Storage)->InventoryGridPanel->
			AddChildToGrid(InventoryWidget->Selector->CurDisplayWidget, GetCoordinateByIndex(SelectorPos, Storage).Y,
				GetCoordinateByIndex(SelectorPos, Storage).X);
		//배치 가능한지 여부 전달
		InventoryWidget->Selector->CurDisplayWidget->SetCanDeploy(
			IsItemAvailableForSlot(SelectorPos, InventoryWidget->Selector->CurItem->GetItemSize(), Storage));
	}
	//잘못된 공간 원래로 돌리기
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
		{//아이템과 공간이 겹칠 경우  아이템의 정보와 위치를 반환 선택 마커 교체
			temp->SetSelectorVisibility(true);
			result = Pos;
			//temp 아이템의 위치 , 크기
			ItemSpace->Key = temp->ItemData->GetItemPos();
			ItemSpace->Value = temp->ItemData->GetItemSize();
		}
		else if (InventoryWidget->Selector->CurDisplayWidget != temp)
		{
			temp->SetSelectorVisibility(false);
		}
	}

	//아이템을 못찾은 경우
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
	//기존 아이템과 일치하는 공간 없음
	if (Pos == FIntPoint{ -1,-1 } && !InventoryWidget->Selector->CurDisplayWidget)
	{
		//보이기
		SelectorVisible(ESlateVisibility::Visible);
		//아이템이 커서위에 없는 경우
		InventoryWidget->DisableSelectInfos();
	}
	//기존 아이템과 일치하는 공간 있는 경우
	else
	{
		//가리기
		SelectorVisible(ESlateVisibility::Hidden);
		if (!InventoryWidget->Selector->CurDisplayWidget)
		{	//아이템클릭 가능한 상황
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
	//선택 위젯 위치 설정
	InventoryWidget->Selector->SetStorage(static_cast<EItemStorageType>(IntStorage));
	//위치 설정
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
	//커서에 아이템이 없는 상태 [기능 동작 필요 없음]
	if (InventoryWidget->Selector->GetVisibility() != ESlateVisibility::Hidden)
	{
		InventoryWidget->Selector->SetItemData(nullptr);
		return;
	}
	//선택위젯에 아이템이 있는 경우
	if (InventoryWidget->Selector->CurDisplayWidget != nullptr)
	{
		SelectMoveItem();
		return;
	}
	//현재 선택 위젯이 가리키는 아이템 선택
	SetCurItem();
	//일반 그리드의 경우
	if (!InventoryWidget->Selector->GetIsPointToEquipmentPlace())
	{
		SelectMoveItem();
	}
}

void UInventoryComponent::UseItemQuantityChange()
{
	//수량 줄이기
	CurUseItem->SetQuantity(CurUseItem->GetQuantity() - 1);
	if (CurUseItem->GetQuantity() > 0)
	{
		//보여지는 수량 업데이트
		FindDisplayByItemData(CurUseItem)->UpdateQuantity();
	}
	//줄일 수량이 없으면 삭제
	else
	{
		RemoveItem(CurUseItem);
	}
}

void UInventoryComponent::SelectMoveItem()
{
	//이미 선택해서 이동 중이면
	if (InventoryWidget->Selector->CurDisplayWidget)
	{
		//배치 가능하면 배치하고 바뀐 정보 저장
		if (InventoryWidget->Selector->CurDisplayWidget->GetCanDeploy())
		{
			//탐색
			UInventoryItemDisplay* CurDisplay = FindDisplayByItemData(InventoryWidget->Selector->CurItem);
			if (!IsValid(CurDisplay))
				return;

			//저장//기존 display 회전
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

			if (CurDisplay->ItemData->GetStoragePos() == GetSelectorStorage())//같은 공간에서 옮길때
			{
				ItemMoveSameGrid(CurDisplay);
			}
			else//다른 공간으로 옮길때
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
		//해당되는 디스플레이 탐색
		UInventoryItemDisplay* CurDisplay = FindDisplayByItemData(CurItem);
		CurDisplay->SetVisibility(ESlateVisibility::Hidden);
		if (CurDisplay->Owner)
		{
			ClearSlots(GetCoordinateByIndex(CurDisplay->GetInventoryIndex(), CurDisplay->Owner->GetStorageType()), CurItem->GetItemSize(), CurDisplay->Owner->GetStorageType());
		}

		//옮기는 것을 보여줄 가상 아이템 디스플레이 생성
		UInventoryItemDisplay* DragVisual = CreateWidget<UInventoryItemDisplay>
			(GetWorld()->GetFirstPlayerController(), ItemDisplayClass);
		DragVisual->Owner = InventoryWidget->GetGrid(GetSelectorStorage());
		DragVisual->InvComp = this;
		DragVisual->Init(CurItem);
		DragVisual->SetIsFocus(true);
		//selector에 저장
		InventoryWidget->Selector->SetItemDisplay(DragVisual);
		DragVisual->SetSelectorVisibility(true);
		//복제본 띄우기
		InventoryWidget->GetGrid(GetSelectorStorage())->InventoryGridPanel->
			AddChildToGrid(InventoryWidget->Selector->CurDisplayWidget, GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).Y,
				GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).X);
	}
}

void UInventoryComponent::DeSelectItem()
{
	if (!InventoryWidget->Selector->CurDisplayWidget)
		return;
	//해당되는 디스플레이 탐색
	UInventoryItemDisplay* CurDisplay = FindDisplayByItemData(CurItem);
	if (!IsValid(CurDisplay))
		return;

	//보이도록 변경
	CurDisplay->SetVisibility(ESlateVisibility::Visible);
	//수량 업데이트
	CurDisplay->UpdateQuantity();
	CurDisplay->SetSelectorVisibility(false);
	FillSlots(CurDisplay->ItemData->GetItemPos(), CurDisplay->ItemData->GetItemSize(), CurItem->GetStoragePos());

	//초기화
	InventoryWidget->Selector->Reset();
	//좌표 초기화
	SelectorPos = GetSlotIndexByCoordinate(CurItem->GetItemPos(), CurItem->GetStoragePos());
	//커서 이동
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
	/* 장착중인 아이템 처리 필요 */
	if (HasBackPack)//백팩
	{
		ItemData->SetStoragePos(EItemStorageType::BackPack);
		if (AddItem(ItemData, EItemStorageType::BackPack))
		{
			return true;
		}
	}
	if (HasSecureContainer)//보안 컨테이너
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
	////sphere 트레이스로 주변의 아이템 확인
	//FVector Start = GetOwner()->GetActorLocation();
	//FVector End = GetOwner()->GetActorLocation();
	//TArray<AActor*> ActorsIgnore;//무시하는 액터 배열
	//TArray<FHitResult> Hits; //충돌 결과를 가지고 있는 배열
	////트레이스 처리
	//const bool Hit = UKismetSystemLibrary::SphereTraceMulti(GetWorld(), Start, End, SearchRange,
	//	UEngineTypes::ConvertToTraceType(ECC_Camera), false, ActorsIgnore, EDrawDebugTrace::ForDuration, Hits, true, FLinearColor::Gray, FLinearColor::Blue, 60.0f);
	//if (Hit)//충돌한 것이 있으면
	//{
	//	for (const FHitResult HitResult : Hits)
	//	{
	//		if (Cast<ABaseItem>(HitResult.GetActor()))
	//		{
	//			ABaseItem* Item = Cast<ABaseItem>(HitResult.GetActor());
	//			//중복은 넘김
	//			if (FieldDropItems.Find(Item->GetUniqueID()) != nullptr)
	//				continue;;
	//			//UID를 통해서 TMap에 저장
	//			FieldDropItems.Add({ Item->GetUniqueID() , Item });
	//			//아이템의 상태를 전달받기 위해서 아이템에 상태를 전달받는 구독자로 등록
	//			Item->AddObserver(Cast<ACharacter>(GetOwner()));
	//			//아이템 추가
	//			Item->ItemData->SetStoragePos(EItemStorageType::Storage);
	//			AddItem(Item->ItemData, EItemStorageType::Storage);
	//		}
	//	}
	//}
	//
	//if (!FieldDropItems.IsEmpty())//비어있지 않으면 활성화
	//	InventoryWidget->StorageGrid->SetVisibility(ESlateVisibility::Visible);
	//else//비어있으면 비활성화
	//	InventoryWidget->StorageGrid->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryComponent::ClearSearchItem()
{
	//for (auto& temp : FieldDropItems)
	//{
	//	//구독 취소
	//	temp.Value->DeleteObserver(Cast<ACharacter>(GetOwner()));
	//	//아이템 디스플레이 삭제
	//	RemoveItem(temp.Value->ItemData);
	//}
	////TMap 초기화
	//FieldDropItems.Empty();
	////위젯 끄기
	////InventoryWidget->StorageGrid->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryComponent::ItemMoveOtherGrid(UInventoryItemDisplay* CurDisplay)
{
	//새로운 그리드에 아이템 추가
	AddItem(GetCurItem(), SelectorPos, GetSelectorStorage());
	//기존 그리드에서 아이템 데이터 삭제
	RemoveItem(CurDisplay->ItemData);
	//인벤토리 디스플레이 제거
	CurDisplay->SetInventoryIndex(SelectorPos);
	FillSlots(GetCoordinateByIndex(SelectorPos, GetSelectorStorage()), GetCurItem()->GetItemSize(), GetSelectorStorage());
	MoveSelector(GetCoordinateByIndex(SelectorPos, GetSelectorStorage()), true);
}

void UInventoryComponent::ItemMoveSameGrid(UInventoryItemDisplay* CurDisplay)
{
	CurDisplay->SetVisibility(ESlateVisibility::Visible);
	//기존 display 회전
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
	//그리드에 저장
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
	//해당되는 디스플레이 탐색
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
	//기존 그리드에서 아이템 데이터 삭제
	RemoveItem(InventoryItemDisplay->ItemData);
}

void UInventoryComponent::ResetInventory()
{
	//그리드 초기화
	InventoryWidget->SecureContainerGrid->InitInventoryWidget();
	InventoryWidget->BackpackGrid->InitInventoryWidget();
	InventoryWidget->StorageGrid->InitInventoryWidget();
	//StorageGrid 가리기
	InventoryWidget->StorageGrid->SetVisibility(ESlateVisibility::Hidden);
}

void UInventoryComponent::ResetInventory(EItemStorageType Storage)
{
	switch (Storage)
	{
	case EItemStorageType::Storage:
		//그리드 초기화
		InventoryWidget->StorageGrid->InitInventoryWidget();
		GetItemContainer(EItemStorageType::Storage).Empty();
		//StorageGrid 가리기
		InventoryWidget->StorageGrid->SetVisibility(ESlateVisibility::Hidden);
		break;
	case EItemStorageType::BackPack:
		//그리드 초기화
		InventoryWidget->BackpackGrid->InitInventoryWidget();
		break;
	case EItemStorageType::SecureContainer:
		//그리드 초기화
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
		//아이템 디스플레이 초기화
		if (InventoryWidget->Selector->GetIsPointToEquipmentPlace())
		{
			//switch (GetSelectorStorage())
			//{
			//case EItemStorageType::PrimaryWeapon:
			//	//비어있지 않으면 장착 해제
			//	if (!InventoryWidget->PrimaryWeapon->GetIsSlotEmpty())
			//	{
			//		InventoryWidget->PrimaryWeapon->RemoveEquipment();
			//	}
			//	break;
			//case EItemStorageType::PrimaryWeapon1:
			//	//비어있지 않으면 장착 해제
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
			//삭제
			RemoveItem(Item);
		}

		//기능 UI 비활성화
		InventoryWidget->DisableSelectInfos();
	}
}

void UInventoryComponent::DropItem_Server_Implementation(const FItemNetInfo& ItemNetInfo)
{
	//아이템 스폰
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
	////위젯에서 사운드 재생
	//DropItemEvent();
}

void UInventoryComponent::DropItemAll()
{
	//모든 아이템 떨구기 실행

	if (Cast<ACharacter>(GetOwner())->IsLocallyControlled())
	{
		for (EItemStorageType Val : TEnumRange<EItemStorageType>())
		{
			if (Val == EItemStorageType::SecureContainer)//보안 컨테이너 빼고 
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

		/*인벤토리 비어있는지 확인*/
		//UpdateTotalItemContainer();
		if (TotalItemContainer.IsEmpty())
			return;

		//서버에 보낼 array 제작
		TArray<FItemNetInfo> ItemNetInfos;
		for (auto& temp : TotalItemContainer)
		{
			FItemNetInfo TempItemNetInfo;

			TempItemNetInfo.ID = temp->GetID();
			TempItemNetInfo.Quantity = temp->GetQuantity();
			ItemNetInfos.Add(TempItemNetInfo);
		}

		//아이템 삭제
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
		//보호 저장공간만큼 백팩의 공간 사용중으로 처리하기
		InventoryWidget->BackpackGrid->InitInventoryWidget();
		//무기&장비창 초기화
		//InventoryWidget->PrimaryWeapon->RemoveEquipment();
		//InventoryWidget->PrimaryWeapon1->RemoveEquipment();

		//배열 보내기
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

	if (ItemDataAsset->IsStackable())//겹치기 가능한 경우
	{
		for (const auto ItemRef : GetItemContainer(StorageType))
		{
			ItemRef->InvComp = this;
			if (ItemRef->ItemData->GetItemName().EqualTo(ItemDataAsset->GetItemName()))
			{
				//최대 수량이 넘어가면 합치기 불가로 변경
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

	//필드 드랍 아이템을 가져오는 경우 필드에서 제거해줘야함
	//ABaseItem* FieldItem =
	
	return true;
}

bool UInventoryComponent::IsItemAvailableForSlot(const int Index, const FIntPoint& ItemSize,
	EItemStorageType StorageType) const
{
	const auto Coordinate = GetCoordinateByIndex(Index, StorageType);

	const int ItemColumnSize = ItemSize.Y;
	const int ItemRowSize = ItemSize.X;

	//빈공간안에 들어갈수있는지 확인
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
		//찾은 경우
		if (temp->ItemData == Data)
			return temp;
		//찾은 경우
		//if (temp->ItemData->GetNetPushId() == Data->GetNetPushId())
		//	return temp;
	}
	//못 찾은 경우
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
		//닫기 BP이벤트
		OpenCloseEvent(IsOpen);
		//근처 아이템 탐색
		SearchItem();
	}
	else
	{
		//근처 아이템 탐색 취소
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

	////열지 않을때
	//if (!IsOpen && CurBodyBag != nullptr)
	//{
	//	//기존의 아이템 리스트 초기화
	//	CurBodyBag->Reset_Server();
	//	if (!GetItemContainer(EItemStorageType::Storage).IsEmpty())//비어있지 않을 경우/* 필드 드랍 아이템 때문에 이 부분 수정해야함
	//	{
	//		//서버에 보낼 array 제작
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
	//	//보관함 초기화
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
	//입력 없이 호출만 하는 경우
	if (InventoryWidget->Selector->GetVisibility() == ESlateVisibility::Hidden)//선택 위젯이 가리키는 것이 있는지 없는지 확인
	{
		for (const auto& temp : GetItemContainer(GetSelectorStorage()))
		{
			//temp 아이템의 위치 , 크기
			if ((temp->ItemData->GetItemPos().X <= GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).X && GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).X < temp->ItemData->GetItemPos().X + temp->ItemData->GetItemSize().X)
				&& (temp->ItemData->GetItemPos().Y <= GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).Y && GetCoordinateByIndex(SelectorPos, GetSelectorStorage()).Y < temp->ItemData->GetItemPos().Y + temp->ItemData->GetItemSize().Y))
			{//아이템과 공간이 겹칠 경우  아이템의 정보와 위치를 반환
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
			//수정 필요
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
