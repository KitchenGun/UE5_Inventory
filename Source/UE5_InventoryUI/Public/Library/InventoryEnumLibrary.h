// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h"
#include "InventoryEnumLibrary.generated.h"

UENUM(BlueprintType)
enum class EItemType : uint8
{
	None = 0,
	Weapon,			//����
	WeaponMods,		//���� ��ǰ
	Ammunition,		//ź��
	Clothing,		//�Ƿ�
	Provisions,		//�ķ�ǰ
	Medical,		//�Ǿ�ǰ
	Loot			//��Ÿ
};

UENUM(BlueprintType)
enum class EWeaponItemSlotType : uint8
{
	None = 0,
	PrimaryWeapon,		//�ֹ��� ����
	SecondaryWeapon,	//�������� ����
	Melee,				//�������� ����
	Grenade				//����ź ����
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	None = 0,
	AR,			//���ݼ���
	LMG,		//������
	SMG,		//�������
	SG,			//����
	DMR,		//�����������
	SR,			//������
	HG			//����
};

UENUM(BlueprintType)
enum class EWeaponModItemType : uint8
{
	None = 0,
	MuzzleDevice,	//�ѱ� ��ǰ(������ ������ �ҿ���)
	Sight,			//���ذ�(��Ʈ����Ʈ ���̾����Ʈ ������)
	Magazines,		//źâ(10 30 40 60 100)
	Foregrip,		//���������(���� �ޱ� �簢��)
	TacticalDevice	//���� ������ (�÷��� ������ ǥ�����ñ�)
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
	//�Ӹ�
	Headgear,
	Eyewear,
	Facecover,
	//����
	Backpack,
	SecureContainers,
	//����
	Tops,
	LeftTops,
	RightTops,
	ChestRigs,
	//����
	Bottoms,
	LeftBottoms,
	RightBottoms,
	//�Ź�
	Shoes
};

UENUM(BlueprintType)
enum class EProvisionItemType : uint8
{
	None = 0,
	Ingredients,	//���
	CannedFood,		//ĵ����
	Fruit,			//����
	FieldRation,	//�����ķ�
	CookedFood		//������ ����
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
	Immediately,	//��� ȸ��
	Progressive	//���������� ��ü ȸ��
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
	//���Ŀ� ����Ҷ� �߰� �ʿ�
};

UCLASS()
class UE5_INVENTORYUI_API UInventoryEnumLibrary : public UUserDefinedEnum
{
	GENERATED_BODY()
	
};
