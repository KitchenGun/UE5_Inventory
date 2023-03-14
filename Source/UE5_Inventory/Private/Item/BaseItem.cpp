#include "Item/BaseItem.h"

#include "UE5_InventoryCharacter.h"
#include "Components/BoxComponent.h"
#include "UE5_InventoryGameInstance.h"
#include "DataAsset/ItemDataAsset.h"
#include "Library/InventoryStructLibrary.h"


ABaseItem::ABaseItem()
{
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	MeshComponent->SetupAttachment(SceneComponent);
	MeshComponent->SetRelativeLocation({ 0,0,0 });

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(MeshComponent);
	BoxCollision->SetRelativeLocation({ 0,0,0 });
	
	SetReplicatingMovement(true);
}

void ABaseItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void ABaseItem::SetItemData(const FItemNetInfo& ItemNetInfo)
{
	SetItemData_Server(ItemNetInfo);
}

void ABaseItem::SetItemData_Server_Implementation(const FItemNetInfo& ItemNetInfo)
{
	FItem NewItem = *Cast<UUE5_InventoryGameInstance>(GetGameInstance())->GetItemInfo(ItemNetInfo.ID);

	ItemData = NewObject<UItemDataAsset>();

	//데이터 테이블 접근해서 받아오기
	ItemData->SetItemInfo(NewItem) ;
	MeshComponent->SetStaticMesh(NewItem.ItemMesh);
	ItemData->SetQuantity(ItemNetInfo.Quantity);
}

void ABaseItem::Interact()
{
	AUE5_InventoryCharacter* Player = Cast<AUE5_InventoryCharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	if(Player==nullptr)
		return;
	Player->GetInteractableObject_Server(Player->FindComponentByClass<UInventoryComponent>(), this);
	//RTT로 인해서 중복으로 들어오지 않게 삭제
	this->Destroy();
}

