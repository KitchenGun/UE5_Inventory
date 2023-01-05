#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "DataAsset/ItemDataAsset.h"
#include "InventoryItemDisplay.generated.h"

class UBorder;
class UTextBlock;
class USizeBox;
class UInventoryGridWidget;
class UInventoryComponent;

UCLASS()
class UE5_INVENTORY_API UInventoryItemDisplay : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY()
		UInventoryGridWidget* Owner = nullptr;
	UPROPERTY(BlueprintReadOnly)
		UInventoryComponent* InvComp = nullptr;
	UPROPERTY(BlueprintReadOnly)
		UItemDataAsset* ItemData = nullptr;

	void Init(UItemDataAsset* BasicItemData);

	//���÷��� ���� ���� ����
	void SetCanDeployColor();//��ġ&���ð���
	void SetInitColor();//�⺻
	void SetCantDeployColor();//��ġ �Ұ���

	void UpdateQuantity();

	UFUNCTION(BlueprintCallable)
		void IsRotate();

	/*GET SET*/
	FORCEINLINE void SetInventoryIndex(const int Index)
	{
		SlotIndexOnInventory = Index;
	};

	UFUNCTION(BlueprintCallable)
		FORCEINLINE int GetInventoryIndex() const
	{
		return SlotIndexOnInventory;
	}

	void SetSelectorVisibility(bool Val)
	{
		if (Selector)
		{
			if (Val)
				Selector->SetVisibility(ESlateVisibility::Visible);
			else
				Selector->SetVisibility(ESlateVisibility::Hidden);
		}
	}

	FIntPoint GetItemSize() const;

	FORCEINLINE int GetTotalSlotForItem() const
	{
		return ItemData->GetItemSize().X * ItemData->GetItemSize().Y;
	}

	FORCEINLINE void SetIsFocus(bool val)
	{
		IsFocus = val;
	}

	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool GetIsFocus()
	{
		return IsFocus;
	}

	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool GetCanDeploy()
	{
		return CanDeploy;
	}

	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetCanDeploy(bool Val)
	{
		CanDeploy = Val;
	}

	UPROPERTY(BlueprintReadWrite, meta = (BindWidget))
		UImage* ItemIcon = nullptr;
	//���콺�� �������� �ִ��� ������

	UPROPERTY(BlueprintReadWrite)
		bool IsMouseUpOnWidget = false;

	UPROPERTY(meta = (BindWidget))
		USizeBox* IconSizeBox;
	UPROPERTY(meta = (BindWidget))
		UBorder* SlotBorder = nullptr;
	UPROPERTY(meta = (BindWidget))
		UImage* Selector;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* StackSizeTextBlock = nullptr;
	UPROPERTY(meta = (BindWidget))
		UTextBlock* NameTextBlock;
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
		FLinearColor BorderInitialColor;
private:
	bool IsFocus = false;

	bool CanDeploy = false;

	int SlotIndexOnInventory = -1;

	UPROPERTY(EditAnywhere, Category = "TileSize")
		int32 TileSize = 50;

	virtual void NativeConstruct() override;

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;


	virtual void NativeOnDragDetected(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;//�巡�������� �߸��� ������ ��� ȣ��Ǵ� �Լ�
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	//���� ������Ʈ ���� ���� ����Ʈ�����
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

};
