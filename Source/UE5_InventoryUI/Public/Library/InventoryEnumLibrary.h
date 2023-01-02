// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "InventoryEnumLibrary.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None = 0,
	Weapon,			//무기
	WeaponMods,		//무기 부품
	Ammunition,		//탄약
	Clothing,		//의류
	Provisions,		//식료품
	Medical,		//의약품
	Loot			//기타
};

UENUM(BlueprintType)
enum class EWeaponItemSlotType : uint8
{
	None = 0,
	PrimaryWeapon,		//주무기 슬롯
	SecondaryWeapon,	//보조무기 슬롯
	Melee,				//근접무기 슬롯
	Grenade				//수류탄 슬롯
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None = 0,
	AR,			//돌격소총
	LMG,		//경기관총
	SMG,		//기관단총
	SG,			//샷건
	DMR,		//지정사수소총
	SR,			//저격총
	HG			//권총
};

UENUM(BlueprintType)
enum class EWeaponModItemType : uint8
{
	None = 0,
	MuzzleDevice,	//총구 부품(소음기 보정기 소염기)
	Sight,			//조준경(도트사이트 아이언사이트 스코프)
	Magazines,		//탄창(10 30 40 60 100)
	Foregrip,		//전방손잡이(수직 앵글 양각대)
	TacticalDevice	//전술 부착물 (플래시 레이져 표적지시기)
};

UENUM(BlueprintType)
enum class EAmmunitionItemType : uint8
{
	None = 0,
	Cartridge9mm,
	Cartridge556mm,
	Cartridge762mm,
	Cartridge308mm,
	Arrow
};

UENUM(BlueprintType)
enum class EClothingItemType : uint8
{
	None = 0,
	//머리
	Headgear,
	Eyewear,
	Facecover,
	//가방
	Backpack,
	SecureContainers,
	//상의
	Tops,
	LeftTops,
	RightTops,
	ChestRigs,
	//하의
	Bottoms,
	LeftBottoms,
	RightBottoms,
	//신발
	Shoes
};

UENUM(BlueprintType)
enum class EProvisionItemType : uint8
{
	None = 0,
	Ingredients,	//재료
	CannedFood,		//캔음식
	Fruit,			//과일
	FieldRation,	//전투식량
	CookedFood		//조리된 음식
};

UENUM(BlueprintType)
enum class EMedicalItemType : uint8
{
	None = 0,
	FirstAidKit,
	Drink,
	Bandage
};

UENUM(BlueprintType)
enum class EMedicalItemUsingType : uint8
{
	None = 0,
	Immediately,	//즉시 회복
	Progressive	//점진적으로 전체 회복
};

UENUM(BlueprintType)
enum class EItemStorageType : uint8
{
	None = 0,
	SecureContainer,
	BackPack,
	Storage,
	PrimaryWeapon,
	Max
	//추후에 사용할때 추가 필요
};

UCLASS()
class UE5_INVENTORYUI_API UInventoryEnumLibrary : public UUserDefinedEnum
{
	GENERATED_BODY()
	
};
