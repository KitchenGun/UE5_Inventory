#pragma once
#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "../../Library/InventoryStructLibrary.h"
#include "InventoryGridWidget.generated.h"

class UGridPanel;
class UCanvasPanel;
class UBorder;
class UOverlay;
class UImage;

class UInventorySlot;
class UInventoryItemDisplay;
class UInventoryComponent;

UCLASS()
class UE5_INVENTORY_API UInventoryGridWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	void InitInventoryWidget();

	virtual void NativePreConstruct() override;

	UPROPERTY(EditInstanceOnly, Category = "Settings", meta = (ClampMin = "0", ClampMax = "50", UIMin = "0", UIMax = "50", ExpseOnSpawn = true))
		int ColumnCount = 10;

	UPROPERTY(EditInstanceOnly, Category = "Settings", meta = (ClampMin = "0", ClampMax = "50", UIMin = "0", UIMax = "50", ExpseOnSpawn = true))
		int RowCount = 10;

	UPROPERTY(EditInstanceOnly, Category = "Settings", meta = (ClampMin = "0", ClampMax = "75", UIMin = "0", UIMax = "75", ExpseOnSpawn = true))
		float TileSize = 50;

	UPROPERTY(EditInstanceOnly, Category = "Class Settings")
		EItemStorageType StorageType;

	void SetInventoryComponent(UInventoryComponent* InvComp);

	//저장소 형태 저장
	EItemStorageType GetStorageType();

	void SetStorageType(EItemStorageType NewStorageType);


	UInventoryComponent* GetInventoryComponent();

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
		UGridPanel* InventoryGridPanel = nullptr;

	UPROPERTY(meta = (BindWidget))
		UBorder* GridPanelBorder = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "Class Settings")
		TSubclassOf<UInventoryItemDisplay> ItemDisplayClass;

	UPROPERTY(EditDefaultsOnly, Category = "Class Settings")
		TSubclassOf<UInventorySlot> InventorySlotClass;

	TArray<UInventorySlot*> Slots;
	TMap<UInventorySlot*, bool> SlotmMap;
	
private:
	UPROPERTY()
		UInventoryComponent* InventoryComponent = nullptr;
};
