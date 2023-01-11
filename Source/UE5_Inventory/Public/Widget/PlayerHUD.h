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

	//크로스헤어 색 변경
	UFUNCTION(BlueprintCallable)
		void SetCrossHairColor(bool bIsDetected);



	UFUNCTION(BlueprintCallable)
	UInventoryWidget* GetInventoryWidget();
	
	ACharacter* Player = nullptr;

	//위젯 캔버스
	UPROPERTY(meta = (BindWidget))
		UCanvasPanel* Canvas;

	//크로스헤어 이미지
	UPROPERTY(meta = (BindWidget))
		UImage* CrossHair;

	UPROPERTY(meta = (BindWidget))
		UInventoryWidget* InventoryWidget;
};
