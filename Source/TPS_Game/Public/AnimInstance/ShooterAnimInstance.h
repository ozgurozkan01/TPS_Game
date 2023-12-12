// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "ShooterAnimInstance.generated.h"

class AShooterCharacter;
/**
 * 
 */
UCLASS()
class TPS_GAME_API UShooterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:
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
};
