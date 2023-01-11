#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUD.generated.h"

class ACharacter;
class UImage;
class UCanvasPanel;
class UInventoryWidget;


UCLASS()
class UE5_INVENTORY_API UPlayerHUD : public UUserWidget
{
	GENERATED_BODY()
		
public:
	virtual void NativeConstruct() override;

	//ũ�ν���� �� ����
	UFUNCTION(BlueprintCallable)
		void SetCrossHairColor(bool bIsDetected);



	UFUNCTION(BlueprintCallable)
	UInventoryWidget* GetInventoryWidget();
	
	ACharacter* Player = nullptr;

	//���� ĵ����
	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* Canvas;

	//ũ�ν���� �̹���
	UPROPERTY(meta = (BindWidget))
		UImage* CrossHair;

	UPROPERTY(meta = (BindWidget))
		UInventoryWidget* InventoryWidget;
};
