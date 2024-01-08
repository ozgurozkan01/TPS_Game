// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Weapon.generated.h"

UENUM()
enum class EWeaponType : uint8
{
	EWT_SubmachineGun UMETA(DisplayName = "SubmachineGun"),
	EWT_AssaultRifle UMETA(DisplayName = "AssaultRifle"),

	EWT_MAX UMETA(DisplayName = "Default"),
};

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

	EWeaponType WeaponType;
	
public:
	void DecremenetAmmo();

	// Setters
	void SetReloadedAmmo(int32& StoredAmmo);
	
	// Getters
	const USkeletalMeshSocket* GetBarrelSocket() const;
	FTransform GetBarrelSocketTransform() const;
	uint8 GetBulletRoom();
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; } 
	FORCEINLINE uint32 GetCurrentAmmo() const { return CurrentAmmoAmount; }
	FORCEINLINE bool HasAmmo() const { return GetCurrentAmmo() > 0; }
	FORCEINLINE bool IsMagazineFull() const { return CurrentAmmoAmount == MaxAmmoAmount; }
};
