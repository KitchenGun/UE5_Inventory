#include "Widget/Invnetory/InventorySlot.h"

void UInventorySlot::SetCoordinate(const int NewRow, const int NewColumn)
{
}

void UInventorySlot::EnableFillorReFill(const bool IsFill)
{
}

void UInventorySlot::DisplayDrawCanDeploy(UInventoryItemDisplay* Display)
{
}

void UInventorySlot::SetStorageType(EItemStorageType type)
{
}

bool UInventorySlot::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}

bool UInventorySlot::NativeOnDragOver(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	return false;
}

int32 UInventorySlot::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	return int32();
}

void UInventorySlot::NativeOnDragLeave(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
}

void UInventorySlot::NativeOnDragCancelled(const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
}
