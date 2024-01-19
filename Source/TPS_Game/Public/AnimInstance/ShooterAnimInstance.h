// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

class AShooterCharacter;

UENUM(BlueprintType)
enum class EOffsetState : uint8
{
	EOS_Aiming UMETA(DisplayName = "Aiming"),
	EOS_Idle UMETA(DisplayName = "Idle"),
	EOS_Reloading UMETA(DisplayName = "Reloading"),
	EOS_InAir UMETA(DisplayName = "InAir"),

	EOS_MAX UMETA(DisplayName = "Default"),
};

UCLASS()
class TPS_GAME_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	UShooterAnimInstance();
	virtual void NativeInitializeAnimation() override;
	UFUNCTION(BlueprintCallable)
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:

	/** Calculate the character aim rotation (Controller Rotation for AI, Camera Look At Rotation for Main Player) */
	FRotator GetAimRotation();
	/** Calculate the rotation character move through */
	FRotator GetMovementRotation();
	/** Set the movement offset yaw value */
	void SetMovementOffsetYaw();
	/** Control body rotation when character stands still */
	void TurnInPlace();
	/** Control body leaning while character is running */
	void Lean();
	/** Set the Offset State tp update aimOffset animation */
	void UpdateOffsetState();
	/** Set the transition variables to update animation transition */
	void UpdateAnimationTransitionVariables();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<AShooterCharacter> ShooterCharacter;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	float Speed;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	float MovementOffsetYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	float LastFrameMovementOffsetYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsInAir;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsAccelerating;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	bool bAiming;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsReloading;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Turn In Place", meta=(AllowPrivateAccess = "true"))
	float TIP_CharacterCurrentYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Turn In Place", meta=(AllowPrivateAccess = "true"))
	float TIP_CharacterLastFrameYaw;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Turn In Place", meta=(AllowPrivateAccess = "true"))
	float RootYawOffset;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Turn In Place", meta=(AllowPrivateAccess = "true"))
	float CurrentDistanceCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Turn In Place", meta=(AllowPrivateAccess = "true"))
	float LastFrameDistanceCurve;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Turn In Place", meta=(AllowPrivateAccess = "true"))
	float AimOffsetPitch;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Turn In Place", meta=(AllowPrivateAccess = "true"))
	EOffsetState OffsetState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lean", meta=(AllowPrivateAccess = "true"))
	FRotator LeanCharacterRotation;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lean", meta=(AllowPrivateAccess = "true"))
	FRotator LeanCharacterRotationLastFrame;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Lean", meta=(AllowPrivateAccess = "true"))
	double LeanDeltaYawOffset;	
};
