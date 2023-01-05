#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Library/InventoryStructLibrary.h"
#include "ItemDataAsset.generated.h"


UCLASS()
class UE5_INVENTORY_API UItemDataAsset : public UDataAsset
{
	GENERATED_BODY()
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	virtual bool IsSupportedForNetworking() const override;

	UFUNCTION(BlueprintCallable)
	int32 GetID()
	{
		return UID;
	};

	UFUNCTION(BlueprintCallable)
	void SetID(int32 val)
	{
		UID = val;
	};

	UFUNCTION(BlueprintCallable)
		const FText& GetItemName() const
	{
		return ItemInfo.ItemName;
	};

	UFUNCTION(BlueprintCallable)
		const FString& GetDescription() const
	{
		return ItemInfo.Description;
	};

	UFUNCTION(BlueprintCallable)
		const EItemType GetItemType() const
	{
		return ItemInfo.ItemType;
	};

	UFUNCTION(BlueprintCallable)
		FORCEINLINE	UTexture2D* GetThumbnail() const
	{
		return ItemInfo.ItemIcon;
	};

	UFUNCTION(BlueprintCallable)
		FORCEINLINE	UStaticMesh* GetMeshComponent() const
	{
		return ItemInfo.ItemMesh;
	};

	UFUNCTION(BlueprintCallable)
		FORCEINLINE	FIntPoint GetItemSize() const
	{
		return ItemInfo.ItemSize;
	};

	UFUNCTION(BlueprintCallable)
		FORCEINLINE	bool IsStackable() const
	{
		return ItemInfo.CanStackable;
	};

	UFUNCTION(BlueprintCallable)
		FORCEINLINE	int GetQuantity() const
	{
		return Quantity;
	}
	UFUNCTION(BlueprintCallable)
		FORCEINLINE	int GetMaxQuantity() const
	{
		return ItemInfo.MaxQuantity;
	}

	UFUNCTION(BlueprintCallable)
		FORCEINLINE	int AddQuantity(const int Size)
	{
		Quantity += Size;
		return Quantity;
	}

	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetQuantity(const int Size)
	{
		Quantity = Size;
	}


	UFUNCTION(BlueprintCallable)
		FORCEINLINE EItemStorageType GetStoragePos()
	{
		return StoragePosition;
	}

	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetStoragePos(EItemStorageType StorageType)
	{
		StoragePosition = StorageType;
	}

	UFUNCTION(BlueprintCallable)
		FORCEINLINE bool GetIsRotate()
	{
		return IsRotate;
	}

	UFUNCTION(BlueprintCallable)
		FORCEINLINE void SetIsRotate(bool val)
	{//회전과 동시에 아이템 사이즈 변경
		ItemInfo.ItemSize = { ItemInfo.ItemSize.Y,ItemInfo.ItemSize.X };

		IsRotate = val;
	}
	UFUNCTION(BlueprintCallable)
		FItem GetItemInfo()
	{
		return ItemInfo;
	}

	FORCEINLINE void SetItemInfo(FItem Item)
	{
		ItemInfo = Item;
	}

	FORCEINLINE void SetItemSize(FIntPoint size)
	{
		ItemInfo.ItemSize = size;
	}
	FORCEINLINE void SetItemPos(FIntPoint pos)
	{
		this->ItemPos = pos;
	}


	FIntPoint GetItemPos()
	{
		return ItemPos;
	}
	UPROPERTY(EditAnywhere)
		bool IsRotate = false;
	UPROPERTY(EditAnywhere)
		FIntPoint ItemPos = { -1,-1 };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		int32 Quantity = 0;

	UPROPERTY(EditAnywhere)
		EItemStorageType StoragePosition = EItemStorageType::None;//아이템의 보관 위치
private:

	UPROPERTY(EditAnywhere,Replicated)
	 uint32 UID;

	UPROPERTY(EditAnywhere, Replicated)
		FItem ItemInfo;

};
