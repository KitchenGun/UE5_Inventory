#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Library/InventoryStructLibrary.h"
#include "InventorySelectorWidget.generated.h"

class UBorder;
class UInventoryComponent;
class UItemDataAsset;
class UInventoryItemDisplay;

UCLASS()
class UE5_INVENTORY_API UInventorySelectorWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetSize(FIntPoint Val, int32 TileSize);//사이즈 변경
	UFUNCTION(BlueprintCallable)
	void SetInventoryIndex(int Index);
	UFUNCTION(BlueprintCallable)
	void SetItemData(UItemDataAsset* Val);
	UFUNCTION(BlueprintCallable)
	void SetItemDisplay(UInventoryItemDisplay* Val);
	UFUNCTION(BlueprintCallable)
	void SetStorage(EItemStorageType Val);

	UFUNCTION(BlueprintCallable)
	void Reset();
	UFUNCTION(BlueprintCallable)
	FORCEINLINE	void SetIsPointToEquipmentPlace(bool Val);
	UFUNCTION(BlueprintCallable)
	FORCEINLINE	bool GetIsPointToEquipmentPlace();

public:
	UPROPERTY(meta = (BindWidget))
		UBorder* BlinkBorder;
	UPROPERTY()
		EItemStorageType StorageType = EItemStorageType::SecureContainer;
	UPROPERTY()
		int32 CurIndex = 0;
	UPROPERTY()
		FIntPoint CurSize;
	UPROPERTY()
		UItemDataAsset* CurItem;
	UPROPERTY()
		UInventoryItemDisplay* CurDisplayWidget;
	UPROPERTY()
		UInventoryComponent* InventoryComponent;
	UPROPERTY()
		bool IsPointToEquipmentPlace = false;


};
