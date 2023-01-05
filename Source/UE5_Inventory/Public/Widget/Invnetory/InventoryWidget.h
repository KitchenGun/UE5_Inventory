// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Library/InventoryEnumLibrary.h"
#include "InventoryWidget.generated.h"

class UCanvasPanel;
class UHorizontalBox;
class UOverlay;
class UTextBlock;
class UVerticalBox;
class UInventoryGridWidget;
class UInventorySelectorWidget;

UCLASS()
class UE5_INVENTORY_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	void InitWidget();
	UFUNCTION()
		UInventoryGridWidget* GetGrid(EItemStorageType StorageType);

	//UI에서 아이템 선택 가능 상태
	void ItemCanSelect();
	//UI에서 아이템 선택 상태
	void ItemSelect(FText ItemName, FString ItemDescription);
	//UI에서 아이템 선택 불가능 상태
	void DisableSelectInfos();

public:
	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* MainInventoryCanvas;
	UPROPERTY(meta = (BindWidget))
		UHorizontalBox* MainHorizontalBox;
	/*장비 장착*/
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* EquipVerticalBox;

	/*인벤토리*/
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* InventoryVerticalBox;
	UPROPERTY(meta = (BindWidget))
		UInventoryGridWidget* SecureContainerGrid;
	UPROPERTY(meta = (BindWidget))
		UInventoryGridWidget* BackpackGrid;

	/*장비칸*/
	//UPROPERTY(meta = (BindWidget))
	//	UInventoryWeaponWidget* PrimaryWeapon;

	/*보관함*/
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* StorageVerticalBox;
	UPROPERTY(meta = (BindWidget))
		UInventoryGridWidget* StorageGrid;


	/*선택 위젯*/
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UInventorySelectorWidget> Selector;

	/*선택 위젯이 선택할경우 사용되는 위젯*/
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UOverlay> SelectInfos;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> SelectItemName;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UTextBlock> SelectItemDescription;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UHorizontalBox> SelectInfoBox;

	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UOverlay> SelectButton;
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UOverlay> RotateButton;
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UOverlay> ResetButton;
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UOverlay> DropButton;
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UOverlay> InteractButton;
};
