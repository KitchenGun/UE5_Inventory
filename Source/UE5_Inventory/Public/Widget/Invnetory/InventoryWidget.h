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

	//UI���� ������ ���� ���� ����
	void ItemCanSelect();
	//UI���� ������ ���� ����
	void ItemSelect(FText ItemName, FString ItemDescription);
	//UI���� ������ ���� �Ұ��� ����
	void DisableSelectInfos();

public:
	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* MainInventoryCanvas;
	UPROPERTY(meta = (BindWidget))
		UHorizontalBox* MainHorizontalBox;
	/*��� ����*/
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* EquipVerticalBox;

	/*�κ��丮*/
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* InventoryVerticalBox;
	UPROPERTY(meta = (BindWidget))
		UInventoryGridWidget* SecureContainerGrid;
	UPROPERTY(meta = (BindWidget))
		UInventoryGridWidget* BackpackGrid;

	/*���ĭ*/
	//UPROPERTY(meta = (BindWidget))
	//	UInventoryWeaponWidget* PrimaryWeapon;

	/*������*/
	UPROPERTY(meta = (BindWidget))
		UVerticalBox* StorageVerticalBox;
	UPROPERTY(meta = (BindWidget))
		UInventoryGridWidget* StorageGrid;


	/*���� ����*/
	UPROPERTY(meta = (BindWidget))
		TObjectPtr<UInventorySelectorWidget> Selector;

	/*���� ������ �����Ұ�� ���Ǵ� ����*/
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
