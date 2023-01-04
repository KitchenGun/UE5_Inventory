#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include  "../../Library/InventoryStructLibrary.h"
#include "InventorySlot.generated.h"

class USlateBrushAsset;
class UBorder;
class UInventoryGridWidget;
class UInventoryComponent;
class UInventoryItemDisplay;

UCLASS()
class UE5_INVENTORY_API UInventorySlot : public UUserWidget
{
	GENERATED_BODY()
public:

	//grid에서 아이템의 위치 지정
	void SetCoordinate(const int NewRow, const int NewColumn);
	//UI에서 사용 가능 장소인지 아닌지 표현을 위해서 사용하는 변수와 함수
	UPROPERTY(EditDefaultsOnly)
		USlateBrushAsset* BrushAsset = nullptr;
	//UI에서 배치 가능한 장소 표현할때 색상
	UPROPERTY(EditDefaultsOnly)
		FLinearColor ValidPlaceColor;
	//UI에서 배치 불가능한 장소 표현할때 색상
	UPROPERTY(EditDefaultsOnly)
		FLinearColor InvalidPlaceColor;
	//직접적으로 색상을 변경할 Border
	UPROPERTY(meta = (BindWidget))
		UBorder* Border = nullptr;
	//마우스가 가리키는 구역에 따라 색상 변화시키는 함수
	void EnableFillorReFill(const bool IsFill);

	//아이템 디스플레이 크기만큼 배치 가능한지 불가능한지 표시
	void DisplayDrawCanDeploy(UInventoryItemDisplay* Display);

	//위치 SET
	void SetIndex(const int Index) { SlotIndex = Index; };
	//위치 GET
	int GetIndex() const { return SlotIndex; }

	//UPROPERTY()
	UInventoryGridWidget* Owner = nullptr;//아이템 슬롯이 들어갈 그리드
	UInventoryComponent* InvComp = nullptr;//인벤토리 컴포넌트
	void SetStorageType(EItemStorageType type);

private:
	EItemStorageType StorageType;

	bool bCanDraw = false;
	//그리드의 위치 관련 변수
	int SlotIndex = -1;
	int Row = -1;
	int Column = -1;

	FVector2D DrawSize = FVector2D::ZeroVector;
	FLinearColor Color;

	int32 TileSize = 50;

	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual bool NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements, int32 LayerId,
		const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
};
