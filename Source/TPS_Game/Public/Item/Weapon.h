// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Weapon.generated.h"

UCLASS()
class TPS_GAME_API AWeapon : public ABaseItem
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaSeconds) override;
	void ThrowWeapon();
protected:
	virtual void BeginPlay() override;
	void StopFalling();
	
private: // Variables

	/** Weapon Ammo values */
	UPROPERTY(EditAnywhere, Category="Weapon Properties", meta=(AllowPrivateAccess = "true"))
	int32 MaxAmmoAmount;
	UPROPERTY(EditAnywhere, Category="Weapon Properties", meta=(AllowPrivateAccess = "true"))
	int32 CurrentAmmoAmount;

	/** Throwing Control Values */
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bIsFalling;

public:
	void DecremenetAmmo();

	void SetReloadedAmmo(int32& StoredAmmo);
	
	// Getters and Setters
	const USkeletalMeshSocket* GetBarrelSocket() const;
	FTransform GetBarrelSocketTransform() const;
	uint8 GetBulletRoom();
	FORCEINLINE uint32 GetCurrentAmmo() const { return CurrentAmmoAmount; }
	FORCEINLINE bool HasAmmo() const { return GetCurrentAmmo() > 0; }
	FORCEINLINE bool IsMagazineFull() const { return CurrentAmmoAmount == MaxAmmoAmount; }
};
