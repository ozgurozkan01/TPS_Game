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

	/** Character Reference to get the scene component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AShooterCharacter> ShooterRef;
	/** Weapon Reference which is animated in the AnimBP */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AWeapon> OwningWeaponRef;
	/** Hand Scene Component that magazine follows */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess="true"))
	FTransform LeftHandSceneCompTransform;
	/** Controller for magazine movement while reloading */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess="true"))
	bool bIsMovingMagazine;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess="true"))
	float SlideDisplacement;	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Animation, meta=(AllowPrivateAccess="true"))
	float RecoilRotation;
};
