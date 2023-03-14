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

	/* ������*/
	//�ʱ�ȭ
	void InitSelector();
	//��ǥ ������Ʈ
	void UpdateSelector();
	//�׸��� ������ ��ǥ���� �������� Ȯ��
	bool IsOverGrid(FIntPoint Pos, EItemStorageType& Storage);
	//���� �Է¿� ���缭 ������ ������ ���÷������� ���� �̵���Ű�� �Լ�(�ش� �Լ��� ���ð��� ���� �̵� �Ѱ�)
	UFUNCTION(BlueprintCallable)
	void MoveSelector(FIntPoint Pos, bool IsDirect = false);
	//���������� �������� ������ �̵��ϴ� ��� ��ġ�� �������� �Ұ������� UI ������Ʈ �Լ�
	void SelectorCurItemDisplayUpdate();
	//���â ��ȸ�� �Լ�(���Ŀ� ���� �ʿ� ����� enum���� ��ȸ�ϵ��� ����)
	void MoveEquipmentSelector(FIntPoint Input);
	//SelectPos�� �̵� ��Ű�� �Լ�
	void DirectMoveSelector(EItemStorageType Storage);
	//�������� ����Ű���� �ƴ��� Ȯ�� �� ����Ű�� ��� �������� ��ġ�� ��ȯ
	FIntPoint SelectorFindItemPos(FIntPoint Pos, TPair<FIntPoint, FIntPoint>* ItemSpace, EItemStorageType Storage);
	//�������� ���¸� ����Ͽ� UI�� �����ϴ� �Լ� //SelectorFindItemPos ����� ������ ���â�� �����ϰ� �ִ� ��
	void SelectorUIUpdate(FIntPoint Pos = { 0,0 });
	//�����Ͱ� ���̴��� �Ⱥ��̴��� ����
	void SelectorVisible(ESlateVisibility Val);
	//������ �׸��� �󿡼� �̵�
	void MoveGrid(int8 IsInputRight);
	//�����ͷ� ������ ����
	UFUNCTION(BlueprintCallable)
		void SelectItem(FVector2D Pos = FVector2D(-1, -1));

	void UseItemQuantityChange();

	//���� ���� �����̱�
	void SelectMoveItem();

	//������ �׸��� ���������� ���
	void DeSelectItem();

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

	bool PickUpItem(ABaseItem* Item);
	bool PickUpItem(UItemDataAsset* ItemData);

	//�ʵ� ��� ������ Ž��
	void SearchItem();
	//�ʵ� ��� ������ �ʱ�ȭ
	void ClearSearchItem();
	//�ʵ� ���������� Ȯ��
	//void FindFieldItem(UItemDataAsset* I)

	//������ �׸��� �̵�
	void ItemMoveOtherGrid(UInventoryItemDisplay* CurDisplay);
	void ItemMoveSameGrid(UInventoryItemDisplay* CurDisplay);


	UFUNCTION()
		void RemoveItem(UItemDataAsset* ItemData);//������ �׸񿡼� ����
	UFUNCTION()
		void GridRemoveItem(UItemDataAsset* ItemData, EItemStorageType StorageType);//������ �׸񿡼� ����
	UFUNCTION()
		void RemoveEquipSlotOrGrid(UInventoryItemDisplay* InventoryItemDisplay);//���� �������� ������ġ�� ���� �ٸ��� ����ó�� �Լ�
	UFUNCTION(BlueprintCallable)
	void ResetInventory();
	void ResetInventory(EItemStorageType Storage);//grid ���¸� ����

	/*�κ��丮 �������� ������ ȣ���ϴ� �Լ�*/
	UFUNCTION(BlueprintImplementableEvent)
		void OpenCloseEvent(bool IsOpen);

	/*�κ��丮 ������ ������ ������ ȣ���ϴ� �Լ�*/
	UFUNCTION(BlueprintImplementableEvent)
		void DropItemEvent();
	/*���� ������ �Լ�*/
	/*!!!���� �̱��� ��ü�� ������ ƨ��!!!*/
	void DropItem(FName ID, int32 Quantity, UItemDataAsset* Item);
	UFUNCTION(Server, Reliable)
		void DropItem_Server(const FItemNetInfo& ItemNetInfo);
	void DropItem_Server_Implementation(const FItemNetInfo& ItemNetInfo);
	UFUNCTION(Server, Reliable)
		void DropItem_NetMultiCast(const FItemNetInfo& ItemNetInfo);
	void DropItem_NetMultiCast_Implementation(const FItemNetInfo& ItemNetInfo);

	/*���� ���� ������ �Լ�*/
	void DropItemAll();
	UFUNCTION()
		void UpdateTotalItemContainer();
	UFUNCTION(Server, Reliable)
		void DropItemAll_Server(const TArray<FItemNetInfo>& Items);
	void DropItemAll_Server_Implementation(const TArray<FItemNetInfo>& Items);

	/*������ ���� & ����*/
	bool AddItem(UItemDataAsset* ItemDataAsset, EItemStorageType StorageType);//��ġ �������� �ڵ����� ������ �߰� �����ϸ� Ȯ���ϰ� �߰��ϴ� �Լ�
	bool AddItem(UItemDataAsset* ItemDataAsset, int SlotIndex, EItemStorageType StorageType);//��ġ �����ϰ� ������ �߰�

	bool IsItemAvailableForSlot(const int Index, const FIntPoint& ItemSize, EItemStorageType StorageType) const;//�������� �� �� �ִ��� Ȯ��
	int GetSlotIndexByCoordinate(const int Column, const int Row, EItemStorageType StorageType) const; //��ǥ�� ���ؼ� �ε��� ���� ��ȯ
	int GetSlotIndexByCoordinate(const FIntPoint Pos, EItemStorageType StorageType) const; //��ǥ�� ���ؼ� �ε��� ���� ��ȯ
	FIntPoint GetCoordinateByIndex(const int Index, EItemStorageType StorageType) const;//�ε����� ���ؼ� ��ǥ ��ȯ

	UInventoryItemDisplay* FindDisplayByItemData(UItemDataAsset* Data);

	void SortItems(EItemStorageType StorageType);//������ ����
	void FillSlots(const FIntPoint& StartPoint, const FIntPoint& ItemSize, EItemStorageType StorageType);
	void ClearSlots(const FIntPoint& StartPoint, const FIntPoint& ItemSize, EItemStorageType StorageType);

	int GetFirstAvailableSlotIndex(const FIntPoint& ItemSize, EItemStorageType StorageType) const;
	/*���� ���� ���� ���� ��*/
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

	//���� �������� �����̰� �ִ� ������ ���÷��� ��ȯ
	UFUNCTION()
		void SelectorItemDisplayRoatate();
	//�κ��丮 �����ִ��� �����ִ��� Ȯ�ο�
	UFUNCTION(BlueprintCallable)
		void SetIsOpenInventory(bool IsOpen);
	UFUNCTION(Server, Reliable)
		void SetIsOpenInventory_Server(bool IsOpen);
	void SetIsOpenInventory_Server_Implementation(bool IsOpen);
	UFUNCTION(NetMulticast, Reliable)
		void SetIsOpenInventory_NetMulticast(bool IsOpen);
	void SetIsOpenInventory_NetMulticast_Implementation(bool IsOpen);

	/*Victor �ӽ� �÷��̾� ������Ʈ�� ��� �ӽ÷� bool�� ó��*/
	UFUNCTION(BlueprintCallable)
		void SetIsUsingMed(bool val);
	UFUNCTION(BlueprintCallable)
		bool GetIsUsingMed();

	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool GetIsOpenInventory()
	{
		return IsOpenInventory;
	}

	//������ �������� ���� ����
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
	
	//����
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

	/* �Է� ���� �Լ� */
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

public:/*����*/
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
	//������ ���� ������ ���
	UPROPERTY(ReplicatedUsing = UpdateTotalItemContainer)
		TArray<UItemDataAsset*> TotalItemContainer;
	//�׸��� ���� ����
	TMap<EItemStorageType, TArray<UInventoryItemDisplay*>> ItemContainer;
	TMap<EItemStorageType, TArray<UInventorySlot*>> ItemSlots;
	TMap<EItemStorageType, TMap<UInventorySlot*, bool>> ItemSlotMap;
	TMap<int32, ABaseItem*> FieldDropItems;

	//�κ��丮 ����
	UInventoryWidget* InventoryWidget = nullptr;

	/*������*/
	//������ ��ġ
	int32 SelectorPos = 0;
	//������Ʈ Ÿ�̸�
	FTimerHandle UpdateSelectorHandler;
	bool IsAlreadyUpdate = false;

	//���������
	UItemDataAsset* CurItem = nullptr;
	//���� ����ϴ� ������ (�κ��丮�� �ݾƵ� �ʱ�ȭ�� �ȵǰ� �ϱ����ؼ� ���)
	UItemDataAsset* CurUseItem = nullptr;

	//�����۵� ���� ����
	UPROPERTY(Replicated)
		float Weight = 0;
	//���� ǥ��
	bool HasBackPack = true;
	bool HasSecureContainer = true;
	UPROPERTY(Replicated)
		bool IsOpenInventory = false;

	//ĳ���� ���°� ��� ���� �ӽ� ����
	bool IsUsingMed = false;

	//Ž�� �Ÿ�
	float SearchRange = 100.0f;
		
};
