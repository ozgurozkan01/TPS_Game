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
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<AShooterCharacter> ShooterCharacter;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	float Speed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsInAir;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Movement, meta=(AllowPrivateAccess = "true"))
	bool bIsAccelerating;
};
