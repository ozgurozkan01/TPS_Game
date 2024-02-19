#pragma once

#include "Engine/DataTable.h"
#include "WeaponTable.generated.h"

USTRUCT(BlueprintType)
struct FWeaponTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	EAmmoType AmmoType;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	int32 CurrentAmmoAmount;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	int32 MazagineCapacity;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	TObjectPtr<USoundBase> PickupSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	TObjectPtr<USoundBase> EquipSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	TObjectPtr<USkeletalMesh> WeaponMesh;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	TObjectPtr<UTexture2D> AmmoIcon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	TObjectPtr<UTexture2D> WeaponIcon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	FString WeaponName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	TObjectPtr<UMaterialInstance> MaterialInstance;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	int32 MaterialIndex;
};
