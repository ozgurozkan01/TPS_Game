// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class USoundCue;

UCLASS()
class TPS_GAME_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:

	/** Input Functions */
	UFUNCTION()
	void Movement(const FInputActionValue& Value);
	UFUNCTION()
	void LookAround(const FInputActionValue& Value);
	UFUNCTION()
	void Fire(const FInputActionValue& Value);
	/** Character Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	
	UPROPERTY(EditDefaultsOnly, Category=Animation)
	TSubclassOf<UAnimInstance> AnimationClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Sound, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USoundCue> FireSoundCue;
	
	/** Input */
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> ShooterInputMapping;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MovementAction;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> FireAction;
public:
	/** Getter Functıons */
	FORCEINLINE TObjectPtr<USpringArmComponent> GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE TObjectPtr<UCameraComponent> GetFollowCamera() const {return FollowCamera; }
	
};