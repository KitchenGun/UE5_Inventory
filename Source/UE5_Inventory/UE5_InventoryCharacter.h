// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "UE5_InventoryCharacter.generated.h"

UCLASS(config=Game)
class AUE5_InventoryCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FollowCamera;
public:
	AUE5_InventoryCharacter();

	virtual void BeginPlay() override;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Input)
	float TurnRateGamepad;

	/*상호작용 라인트레이스*/
	void Interact();
	/*UI BP에서 처리*/
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateCrossHair();


	/* 입력 관련 함수 */
	UFUNCTION(BlueprintCallable)
	void IA_Jump(bool Value);
	UFUNCTION(BlueprintCallable)
	void IA_LookUpDown(float Value);
	UFUNCTION(BlueprintCallable)
		void IA_TurnRightLeft(float Value);
	UFUNCTION(BlueprintCallable)
		void IA_MoveForwardBackward(float Value);
	UFUNCTION(BlueprintCallable)
		void IA_MoveRightLeft(float Value);
protected:

	/** Called for forwards/backward input */
	void MoveForward(float Value);

	/** Called for side to side input */
	void MoveRight(float Value);

	/** 
	 * Called via input to turn at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate. 
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	/** Handler for when a touch input begins. */
	void TouchStarted(ETouchIndex::Type FingerIndex, FVector Location);

	/** Handler for when a touch input stops. */
	void TouchStopped(ETouchIndex::Type FingerIndex, FVector Location);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	// End of APawn interface

public:
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	/*상호작용 오브젝트를 바라보는 경우*/
	UPROPERTY(EditAnywhere,BlueprintGetter="GetIsInteraction")
	bool CanInteraction = false;

	UFUNCTION(BlueprintGetter)
	FORCEINLINE bool GetIsInteraction()
	{
		return CanInteraction;
	}
	/*상호작용 타이머*/
	FTimerHandle InteractHandler;
};

