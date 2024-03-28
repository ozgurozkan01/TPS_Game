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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	FName MagazineBoneName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	FName ReloadMontageSection;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	TSubclassOf<UAnimInstance> AnimInstanceBP;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	TObjectPtr<UTexture> CrosshairMiddle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	TObjectPtr<UTexture> CrosshairLeft;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	TObjectPtr<UTexture> CrosshairRight;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	TObjectPtr<UTexture> CrosshairBottom;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	TObjectPtr<UTexture> CrosshairTop;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	float AutoFireRate;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	TObjectPtr<USoundBase> FireSoundCue;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	FName BoneToHide;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties")
	bool bIsAutomatic;	
};
