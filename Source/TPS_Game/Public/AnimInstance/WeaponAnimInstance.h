// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "WeaponAnimInstance.generated.h"

class AShooterCharacter;
class AWeapon;
class USceneComponent;

UCLASS()
class TPS_GAME_API UWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AShooterCharacter> ShooterRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AWeapon> OwningWeaponRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess="true"))
	FTransform LeftHandSceneCompTransform;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess="true"))
	bool bIsMovingMagazine;
};
