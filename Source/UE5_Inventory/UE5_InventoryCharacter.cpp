// Copyright Epic Games, Inc. All Rights Reserved.

#include "UE5_InventoryCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "DataAsset/ItemDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Item/BaseItem.h"
#include "Kismet/GameplayStatics.h"

//////////////////////////////////////////////////////////////////////////
// AUE5_InventoryCharacter

AUE5_InventoryCharacter::AUE5_InventoryCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// set our turn rate for input
	TurnRateGamepad = 50.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	//component
	InventoryComponent = FindComponentByClass<UInventoryComponent>();
}

void AUE5_InventoryCharacter::BeginPlay()
{
	Super::BeginPlay();

	//InteractTimer// ����� ���� �ݺ�
	GetWorldTimerManager().SetTimer(InteractHandler, this, &AUE5_InventoryCharacter::Interact, 0.1f, true);
}


void AUE5_InventoryCharacter::Interact()
{
	//�������� �ٶ󺸴��� Ȯ��
	CanInteraction=(InteractLineTraceFunc()==nullptr?false:true);
	//crosshair update
	UpdateCrossHair();
}

AActor* AUE5_InventoryCharacter::InteractLineTraceFunc()
{
	FVector2D CrossHairLocation = FVector2D(0.5f, 0.5f);
	float TraceLength = 600.0f;

	FHitResult HitResult;

	/* ���� Ʈ���̽��� ����� ������ �ش� ����κ��� ���͸� ��´�. */
	FVector CrossHairWorldPosition;
	FVector CrossHairWorldDirection;
	UGameplayStatics::DeprojectScreenToWorld(GetWorld()->GetFirstPlayerController(), FVector2D(GEngine->GameViewport->Viewport->GetSizeXY() / 2.0f), CrossHairWorldPosition, CrossHairWorldDirection);
	/*���� ��ġ�� ������ġ ���*/
	FVector Start = CrossHairWorldPosition;
	FVector End = CrossHairWorldPosition + (CrossHairWorldDirection * TraceLength);

	if (!GetWorld()->LineTraceSingleByProfile(HitResult, Start, End, FName("Item")))
	{
		/* ���� Ʈ���̽��� ����� ������ HitResult�� Location�� Ʈ���̽� ��ǥ����(End)���� ���� */
		HitResult.Location = FVector_NetQuantize(End);
	}

	/* ����Ʈ���̽��� ������ ������Ʈ�� �ִٸ� */
	if (HitResult.bBlockingHit == 1)
	{
		return HitResult.GetActor();
	}
	else
	{
		return nullptr;
	}
}

void AUE5_InventoryCharacter::GetInteractableObject_Server_Implementation(UInventoryComponent* InvComp,
	AActor* ItemActor)
{
	GetInteractableObject_Client(InvComp, ItemActor);
}

void AUE5_InventoryCharacter::GetInteractableObject_Client_Implementation(UInventoryComponent* InvComp,
	AActor* ItemActor)
{
	if (IsValid(ItemActor))//������
	{
		//InvComp->AddWeight(ItemActor->ItemData->GetItemInfo().Weight * ItemActor->ItemData->GetQuantity());
		ItemActor->Destroy();
	}
}

void AUE5_InventoryCharacter::IA_Jump(bool Value)
{
	if(Value)
		Jump();
	else
		StopJumping();
}

void AUE5_InventoryCharacter::IA_LookUpDown(float Value)
{
	AddControllerPitchInput(Value);
}

void AUE5_InventoryCharacter::IA_TurnRightLeft(float Value)
{
	AddControllerYawInput(Value);
}

void AUE5_InventoryCharacter::IA_MoveForwardBackward(float Value)
{
	MoveForward(Value);
}

void AUE5_InventoryCharacter::IA_MoveRightLeft(float Value)
{
	MoveRight(Value);
}

void AUE5_InventoryCharacter::IA_Interact()
{
	ABaseItem* Item = Cast<ABaseItem>(InteractLineTraceFunc());
	//�������� ���
	if(IsValid(Item))
	{	//��ȣ�ۿ�
		Item->Interact();
	}
}

//////////////////////////////////////////////////////////////////////////
// Input

void AUE5_InventoryCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	//PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	//PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	//PlayerInputComponent->BindAxis("Move Forward / Backward", this, &AUE5_InventoryCharacter::MoveForward);
	//PlayerInputComponent->BindAxis("Move Right / Left", this, &AUE5_InventoryCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	//PlayerInputComponent->BindAxis("Turn Right / Left Mouse", this, &APawn::AddControllerYawInput);
	//PlayerInputComponent->BindAxis("Turn Right / Left Gamepad", this, &AUE5_InventoryCharacter::TurnAtRate);
	//PlayerInputComponent->BindAxis("Look Up / Down Mouse", this, &APawn::AddControllerPitchInput);
	//PlayerInputComponent->BindAxis("Look Up / Down Gamepad", this, &AUE5_InventoryCharacter::LookUpAtRate);

	// handle touch devices
	//PlayerInputComponent->BindTouch(IE_Pressed, this, &AUE5_InventoryCharacter::TouchStarted);
	//PlayerInputComponent->BindTouch(IE_Released, this, &AUE5_InventoryCharacter::TouchStopped);
}

void AUE5_InventoryCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
	Jump();
}

void AUE5_InventoryCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
	StopJumping();
}

void AUE5_InventoryCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AUE5_InventoryCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * TurnRateGamepad * GetWorld()->GetDeltaSeconds());
}

void AUE5_InventoryCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AUE5_InventoryCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}
