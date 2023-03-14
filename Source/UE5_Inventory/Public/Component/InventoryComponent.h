#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InputMappingContext.h"
#include "InventoryComponent.generated.h"


class UInventoryItemDisplay;
class UInventoryWidget;
class UInventorySlot;
class UItemDataAsset;
class ABaseItem;

USTRUCT(BlueprintType)
struct FItemNetInfo
{
	GENERATED_BODY()
public:
	UPROPERTY()
		FName ID;
	UPROPERTY()
		int32 Quantity;
};

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE5_INVENTORY_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInventoryComponent();

	UFUNCTION(BlueprintCallable)
	void Init();

	/*tarray replicate*/
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	/*tarray replicate*/

	/* 셀렉터*/
	//초기화
	void InitSelector();
	//좌표 업데이트
	void UpdateSelector();
	//그리드 밖으로 좌표에서 나갔는지 확인
	bool IsOverGrid(FIntPoint Pos, EItemStorageType& Storage);
	//들어온 입력에 맞춰서 위젯과 아이템 디스플레이위젯 같이 이동시키는 함수(해당 함수로 선택관련 위젯 이동 총괄)
	UFUNCTION(BlueprintCallable)
	void MoveSelector(FIntPoint Pos, bool IsDirect = false);
	//선택위젯이 아이템을 가지고 이동하는 경우 배치가 가능한지 불가능한지 UI 업데이트 함수
	void SelectorCurItemDisplayUpdate();
	//장비창 순회용 함수(추후에 수정 필요 현재는 enum으로 순회하도록 설정)
	void MoveEquipmentSelector(FIntPoint Input);
	//SelectPos로 이동 시키는 함수
	void DirectMoveSelector(EItemStorageType Storage);
	//아이템을 가리키는지 아닌지 확인 후 가리키는 경우 아이템의 위치를 반환
	FIntPoint SelectorFindItemPos(FIntPoint Pos, TPair<FIntPoint, FIntPoint>* ItemSpace, EItemStorageType Storage);
	//셀렉터의 상태를 기반하여 UI를 수정하는 함수 //SelectorFindItemPos 결과가 없으면 장비창을 선택하고 있는 것
	void SelectorUIUpdate(FIntPoint Pos = { 0,0 });
	//셀렉터가 보이는지 안보이는지 설정
	void SelectorVisible(ESlateVisibility Val);
	//셀렉터 그리드 상에서 이동
	void MoveGrid(int8 IsInputRight);
	//셀렉터로 아이템 선택
	UFUNCTION(BlueprintCallable)
		void SelectItem(FVector2D Pos = FVector2D(-1, -1));

	void UseItemQuantityChange();

	//선택 위젯 움직이기
	void SelectMoveItem();

	//셀렉터 그리드 선택했을때 취소
	void DeSelectItem();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	bool PickUpItem(ABaseItem* Item);
	bool PickUpItem(UItemDataAsset* ItemData);

	//필드 드랍 아이템 탐색
	void SearchItem();
	//필드 드랍 아이템 초기화
	void ClearSearchItem();
	//필드 아이템인지 확인
	//void FindFieldItem(UItemDataAsset* I)

	//아이템 그리드 이동
	void ItemMoveOtherGrid(UInventoryItemDisplay* CurDisplay);
	void ItemMoveSameGrid(UInventoryItemDisplay* CurDisplay);


	UFUNCTION()
		void RemoveItem(UItemDataAsset* ItemData);//아이템 항목에서 제거
	UFUNCTION()
		void GridRemoveItem(UItemDataAsset* ItemData, EItemStorageType StorageType);//아이템 항목에서 제거
	UFUNCTION()
		void RemoveEquipSlotOrGrid(UInventoryItemDisplay* InventoryItemDisplay);//기존 아이템의 저장위치에 따라서 다르게 삭제처리 함수
	UFUNCTION(BlueprintCallable)
	void ResetInventory();
	void ResetInventory(EItemStorageType Storage);//grid 형태만 구현

	/*인벤토리 여닫을때 위젯에 호출하는 함수*/
	UFUNCTION(BlueprintImplementableEvent)
		void OpenCloseEvent(bool IsOpen);

	/*인벤토리 아이템 버릴때 위젯에 호출하는 함수*/
	UFUNCTION(BlueprintImplementableEvent)
		void DropItemEvent();
	/*물건 떨구는 함수*/
	/*!!!주의 싱글톤 객체가 없으면 튕김!!!*/
	void DropItem(FName ID, int32 Quantity, UItemDataAsset* Item);
	UFUNCTION(Server, Reliable)
		void DropItem_Server(const FItemNetInfo& ItemNetInfo);
	void DropItem_Server_Implementation(const FItemNetInfo& ItemNetInfo);
	UFUNCTION(Server, Reliable)
		void DropItem_NetMultiCast(const FItemNetInfo& ItemNetInfo);
	void DropItem_NetMultiCast_Implementation(const FItemNetInfo& ItemNetInfo);

	/*물건 전부 떨구는 함수*/
	void DropItemAll();
	UFUNCTION()
		void UpdateTotalItemContainer();
	UFUNCTION(Server, Reliable)
		void DropItemAll_Server(const TArray<FItemNetInfo>& Items);
	void DropItemAll_Server_Implementation(const TArray<FItemNetInfo>& Items);

	/*아이템 저장 & 정렬*/
	bool AddItem(UItemDataAsset* ItemDataAsset, EItemStorageType StorageType);//위치 지정없이 자동으로 아이템 추가 가능하면 확인하고 추가하는 함수
	bool AddItem(UItemDataAsset* ItemDataAsset, int SlotIndex, EItemStorageType StorageType);//위치 지정하고 아이템 추가

	bool IsItemAvailableForSlot(const int Index, const FIntPoint& ItemSize, EItemStorageType StorageType) const;//아이템이 들어갈 수 있는지 확인
	int GetSlotIndexByCoordinate(const int Column, const int Row, EItemStorageType StorageType) const; //좌표를 통해서 인덱스 정보 반환
	int GetSlotIndexByCoordinate(const FIntPoint Pos, EItemStorageType StorageType) const; //좌표를 통해서 인덱스 정보 반환
	FIntPoint GetCoordinateByIndex(const int Index, EItemStorageType StorageType) const;//인덱스를 통해서 좌표 반환

	UInventoryItemDisplay* FindDisplayByItemData(UItemDataAsset* Data);

	void SortItems(EItemStorageType StorageType);//아이템 정렬
	void FillSlots(const FIntPoint& StartPoint, const FIntPoint& ItemSize, EItemStorageType StorageType);
	void ClearSlots(const FIntPoint& StartPoint, const FIntPoint& ItemSize, EItemStorageType StorageType);

	int GetFirstAvailableSlotIndex(const FIntPoint& ItemSize, EItemStorageType StorageType) const;
	/*여기 밑은 전부 복사 끝*/
	/*GET&SET*/
	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetHasBackPack(bool val)
	{
		HasBackPack = val;
	}

	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetHasSecureContainer(bool val)
	{
		HasBackPack = val;
	}

	UFUNCTION(BlueprintCallable)
		void SetWidget(UInventoryWidget* Widget)
	{
		InventoryWidget = Widget;
	}

	//선택 위젯으로 움직이고 있는 아이템 디스플레이 반환
	UFUNCTION()
		void SelectorItemDisplayRoatate();
	//인벤토리 열려있는지 닫쳐있는지 확인용
	UFUNCTION(BlueprintCallable)
		void SetIsOpenInventory(bool IsOpen);
	UFUNCTION(Server, Reliable)
		void SetIsOpenInventory_Server(bool IsOpen);
	void SetIsOpenInventory_Server_Implementation(bool IsOpen);
	UFUNCTION(NetMulticast, Reliable)
		void SetIsOpenInventory_NetMulticast(bool IsOpen);
	void SetIsOpenInventory_NetMulticast_Implementation(bool IsOpen);

	/*Victor 임시 플레이어 스테이트가 없어서 임시로 bool로 처리*/
	UFUNCTION(BlueprintCallable)
		void SetIsUsingMed(bool val);
	UFUNCTION(BlueprintCallable)
		bool GetIsUsingMed();

	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool GetIsOpenInventory()
	{
		return IsOpenInventory;
	}

	//위젯에 보여지는 저장 정보
	TArray<UInventoryItemDisplay*>& GetItemContainer(EItemStorageType StorageType)
	{
		return ItemContainer[StorageType];
	}
	TArray<UInventorySlot* >& GetItemSlots(EItemStorageType StorageType)
	{
		return ItemSlots[StorageType];
	}
	TMap<UInventorySlot*, bool>& GetItemSlotmMap(EItemStorageType StorageType)
	{
		return ItemSlotMap[StorageType];
	}
	void SetItemContainer(EItemStorageType StorageType, TArray<UInventoryItemDisplay*> Container)
	{
		if (ItemContainer.IsEmpty())
		{
			ItemContainer.Add(StorageType, Container);
		}
		else
		{
			ItemContainer[StorageType] = Container;
		}
	}
	void SetItemSlots(EItemStorageType StorageType, TArray<UInventorySlot*> Slots)
	{
		if (ItemSlots.IsEmpty())
		{
			ItemSlots.Add(StorageType, Slots);
		}
		else
		{
			ItemSlots[StorageType] = Slots;
		}
	}
	void SetItemSlotMap(EItemStorageType StorageType, TMap<UInventorySlot*, bool> SlotMap)
	{
		if (ItemSlotMap.IsEmpty())
		{
			ItemSlotMap.Add(StorageType, SlotMap);
		}
		else
		{
			ItemSlotMap[StorageType] = SlotMap;
		}
	}

	//curItem
	UFUNCTION(BlueprintCallable)
		FORCEINLINE UItemDataAsset* GetCurItem()
	{
		return CurItem;
	}

	UFUNCTION(BlueprintCallable)
		FORCEINLINE UItemDataAsset* GetCurUseItem()
	{
		return CurUseItem;
	}

	void SetCurItem(UItemDataAsset* cur = nullptr);
	
	//무게
	FORCEINLINE float GetWeight() const
	{
		return Weight;
	}
	void AddWeight(float AddItemWeight)
	{
		Weight += AddItemWeight;
	}

	FORCEINLINE FIntPoint GetGridSize(EItemStorageType Storage);

	FORCEINLINE EItemStorageType GetSelectorStorage();

	FORCEINLINE void SetSelectorStorage(EItemStorageType Storage);


	//widget
	UInventoryWidget* GetInvWidget()
	{
		return InventoryWidget;
	}

	/* 입력 관련 함수 */
	UFUNCTION(BlueprintCallable)
		void IA_InventoryMoveUp();
	UFUNCTION(BlueprintCallable)
		void IA_InventoryMoveDown();
	UFUNCTION(BlueprintCallable)
		void IA_InventoryMoveRight();
	UFUNCTION(BlueprintCallable)
		void IA_InventoryMoveLeft();
	UFUNCTION(BlueprintCallable)
		void IA_InventoryGridChange(float value);
	UFUNCTION(BlueprintCallable)
		void IA_InventoryItemSelect();
	UFUNCTION(BlueprintCallable)
		void IA_InventoryItemDeSelectOrClose();
	UFUNCTION(BlueprintCallable)
		void IA_InventoryItemRotate();
	UFUNCTION(BlueprintCallable)
		void IA_InventoryDrop();

public:/*변수*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Blueprint Base")
	TSubclassOf<ABaseItem> ItemBlueprint;
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Blueprint Base")
	//	TSubclassOf<ABodyBag> BodyBagBlueprint;
	UPROPERTY(EditDefaultsOnly, Category = "Class Settings")
	TSubclassOf<UInventoryItemDisplay> ItemDisplayClass;
	UPROPERTY(EditDefaultsOnly, Category = "Class Settings")
	TSubclassOf<UInventorySlot> InventorySlotClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	UInputMappingContext* InvenInputMappingContext;

private:
	//아이템 전부 저장한 목록
	UPROPERTY(ReplicatedUsing = UpdateTotalItemContainer)
		TArray<UItemDataAsset*> TotalItemContainer;
	//그리드 저장 정보
	TMap<EItemStorageType, TArray<UInventoryItemDisplay*>> ItemContainer;
	TMap<EItemStorageType, TArray<UInventorySlot*>> ItemSlots;
	TMap<EItemStorageType, TMap<UInventorySlot*, bool>> ItemSlotMap;
	TMap<int32, ABaseItem*> FieldDropItems;

	//인벤토리 위젯
	UInventoryWidget* InventoryWidget = nullptr;

	/*셀렉터*/
	//셀렉터 위치
	int32 SelectorPos = 0;
	//업데이트 타이머
	FTimerHandle UpdateSelectorHandler;
	bool IsAlreadyUpdate = false;

	//현재아이템
	UItemDataAsset* CurItem = nullptr;
	//현재 사용하는 아이템 (인벤토리를 닫아도 초기화가 안되게 하기위해서 사용)
	UItemDataAsset* CurUseItem = nullptr;

	//아이템들 전부 무게
	UPROPERTY(Replicated)
		float Weight = 0;
	//상태 표시
	bool HasBackPack = true;
	bool HasSecureContainer = true;
	UPROPERTY(Replicated)
		bool IsOpenInventory = false;

	//캐릭터 상태가 없어서 만든 임시 변수
	bool IsUsingMed = false;

	//탐색 거리
	float SearchRange = 100.0f;
		
};
