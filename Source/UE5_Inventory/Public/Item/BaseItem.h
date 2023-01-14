#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Component/InventoryComponent.h"
#include "BaseItem.generated.h"

class UItemDataAsset;
class UBoxComponent;
class UStaticMeshComponent;
class USceneComponent;

UCLASS()
class UE5_INVENTORY_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseItem();
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
		UItemDataAsset* ItemData = nullptr;
	UFUNCTION()
		void SetItemData(const FItemNetInfo& ItemNetInfo);

	UFUNCTION(NetMulticast, Reliable)
	void SetItemData_Server(const FItemNetInfo& ItemNetInfo);
	void SetItemData_Server_Implementation(const FItemNetInfo& ItemNetInfo);

	UFUNCTION()
		void Interact();

	//UFUNCTION()//������ ������ ������ �߰�
	//	void AddObserver(ACharacter* Player);
	//UFUNCTION()//������ ����
	//	void DeleteObserver(ACharacter* Player);
private:
	//������ ���� ������ �迭
	//TArray<ACharacter*> Observer;

	UPROPERTY(EditInstanceOnly)
		UStaticMeshComponent* MeshComponent = nullptr;

	UPROPERTY()
		UBoxComponent* BoxCollision = nullptr;

	UPROPERTY()
		USceneComponent* SceneComponent = nullptr;
};
