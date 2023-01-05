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

	//디스플레이 상태 색상 변경
	void SetCanDeployColor();//배치&선택가능
	void SetInitColor();//기본
	void SetCantDeployColor();//배치 불가능

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
	//마우스가 위젯위에 있는지 없는지

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
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;//드래그했을때 잘못된 영역일 경우 호출되는 함수
	virtual void NativeOnDragEnter(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	//같은 오브젝트 끼리 겹쳐 떨어트릴경우
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

};
