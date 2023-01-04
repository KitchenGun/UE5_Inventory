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

	//grid���� �������� ��ġ ����
	void SetCoordinate(const int NewRow, const int NewColumn);
	//UI���� ��� ���� ������� �ƴ��� ǥ���� ���ؼ� ����ϴ� ������ �Լ�
	UPROPERTY(EditDefaultsOnly)
		USlateBrushAsset* BrushAsset = nullptr;
	//UI���� ��ġ ������ ��� ǥ���Ҷ� ����
	UPROPERTY(EditDefaultsOnly)
		FLinearColor ValidPlaceColor;
	//UI���� ��ġ �Ұ����� ��� ǥ���Ҷ� ����
	UPROPERTY(EditDefaultsOnly)
		FLinearColor InvalidPlaceColor;
	//���������� ������ ������ Border
	UPROPERTY(meta = (BindWidget))
		UBorder* Border = nullptr;
	//���콺�� ����Ű�� ������ ���� ���� ��ȭ��Ű�� �Լ�
	void EnableFillorReFill(const bool IsFill);

	//������ ���÷��� ũ�⸸ŭ ��ġ �������� �Ұ������� ǥ��
	void DisplayDrawCanDeploy(UInventoryItemDisplay* Display);

	//��ġ SET
	void SetIndex(const int Index) { SlotIndex = Index; };
	//��ġ GET
	int GetIndex() const { return SlotIndex; }

	//UPROPERTY()
	UInventoryGridWidget* Owner = nullptr;//������ ������ �� �׸���
	UInventoryComponent* InvComp = nullptr;//�κ��丮 ������Ʈ
	void SetStorageType(EItemStorageType type);

private:
	EItemStorageType StorageType;

	bool bCanDraw = false;
	//�׸����� ��ġ ���� ����
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
