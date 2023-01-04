#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Engine/UserDefinedStruct.h"
#include "InventoryEnumLibrary.h"
#include "InventoryStructLibrary.generated.h"


class UStaticMesh;
class UItemDataAsset;

USTRUCT(BlueprintType)
struct FItem : public FTableRowBase
{
	GENERATED_BODY()

public:
	//NeedDef 생성전에 정의해야하는 부분
	//앞에 붙은 아이템 종류에 따라 해당 종류면 무조건 DT에서 정의해줘야하는 변수
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemNeedDef)
		EItemType ItemType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemNeedDef, meta = (ClampMin = "1", ClampMax = "10", UIMin = "1", UIMax = "10"))
		FIntPoint ItemSize;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemNeedDef)
		FText ItemName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemNeedDef)
		FString Description;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemNeedDef)
		UTexture2D* ItemIcon;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemNeedDef)
		UStaticMesh* ItemMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemNeedDef)
		int32 MaxQuantity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemNeedDef)
		float Weight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ItemNeedDef)
		bool CanStackable = false;

	//Weapon	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponItemNeedDef)
		EWeaponItemSlotType WeaponSlotType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponItemNeedDef)
		EWeaponType WeaponType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponItemNeedDef)
		int32 BulletVelocity;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponItem)
		bool IsFunction = true;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponItem)
		int32 WeaponDurability;

	//WeaponPart
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponPartItemNeedDef)
		EWeaponModItemType WeaponModType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponPartItemNeedDef)
		TArray<EWeaponModItemType> AdditionalAttachmentTypes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = WeaponPartItem)
		int32 WeaponModDurability;
	//Ammo
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = AmmunitionItemNeedDef)
		EAmmunitionItemType AmmoType;

	//Clothing
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ClothingItemNeedDef)
		EClothingItemType ClothingType;

	//Provisions
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProvisionsItem)
		EProvisionItemType ProvisionType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = ProvisionsItem)
		bool IsRotten;

	//Medical
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MedicalItem)
		EMedicalItemType MedicalType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MedicalItem)
		EMedicalItemUsingType MedicalUsingType;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MedicalItem)
		float UsingTime;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MedicalItem)
		int32 RecoveryCount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MedicalItem)
		float HPRecoveryAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MedicalItem)
		float StaminaRecoveryAmount;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MedicalItem)
		float TemperatureRecoveryAmount;
};

UCLASS()
class UE5_INVENTORY_API UInventoryStructLibrary : public UUserDefinedStruct
{
	GENERATED_BODY()

};
