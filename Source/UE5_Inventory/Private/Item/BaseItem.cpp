#include "Item/BaseItem.h"
#include "Components/BoxComponent.h"


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
	BoxCollision->bHiddenInGame = false;

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
	//FItem NewItem = *Cast<GameInstance>(GetGameInstance())->GetItemInfo(FName(FString::FromInt(ItemNetInfo.ID)));
	//데이터 테이블 접근해서 받아오기
	//ItemData = NewItem.DataAsset;
	//MeshComponent->SetStaticMesh(NewItem.ItemMesh);
	//ItemData->SetQuantity(ItemNetInfo.Quantity);
}

void ABaseItem::Interact()
{
	//ACharacter* Player = Cast<ACharacter>(GetWorld()->GetFirstPlayerController()->GetCharacter());
	//Player->GetInteractableObject_Server(Player->FindComponentByClass<UInventoryComponent>(), this);
}

