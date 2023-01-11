#include "Widget/PlayerHUD.h"

#include "Components/Image.h"

void UPlayerHUD::NativeConstruct()
{
	Super::NativeConstruct();
	Player = GetWorld()->GetFirstPlayerController()->GetCharacter();
}

void UPlayerHUD::SetCrossHairColor(bool bIsDetected)
{
	if (bIsDetected)
	{
		CrossHair->SetColorAndOpacity(FLinearColor(FColor::Red));
	}
	else
	{
		CrossHair->SetColorAndOpacity(FLinearColor(FColor::White));
	}
}

UInventoryWidget* UPlayerHUD::GetInventoryWidget()
{
	return InventoryWidget;
}

