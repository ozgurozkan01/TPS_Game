// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "WeaponRarityTable.h"
#include "Weapon.generated.h"

enum class EAmmoType : uint8;

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

	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;
	
private: // Variables
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInformationPopUp> InformationWidgetObject;
	/** Weapon Ammo values */
	UPROPERTY(EditAnywhere, Category="Weapon Properties", meta=(AllowPrivateAccess = "true"))
	int32 MagazineCapacity;
	UPROPERTY(EditAnywhere, Category="Weapon Properties", meta=(AllowPrivateAccess = "true"))
	int32 CurrentAmmoAmount;

	/** Throwing Control Values */
	FTimerHandle ThrowWeaponTimer;
	float ThrowWeaponTime;
	bool bIsFalling;

	EWeaponType WeaponType;
	EAmmoType AmmoType;
	FName ReloadingMontageSection;
	FName MagazineBoneName;

	bool bIsMovingMagazine;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon Properties", meta=(AllowPrivateAccess = "true"))
	int32 SlotIndex;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Weapon Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UDataTable> WeaponRarityTable;

	/** Data Table Variables */
	UPROPERTY(EditDefaultsOnly, Category="Table Property")
	FLinearColor GlowColor;
	UPROPERTY(EditDefaultsOnly, Category="Table Property")
	FLinearColor BrightColor;
	UPROPERTY(EditDefaultsOnly, Category="Table Property")
	FLinearColor DarkColor;
	UPROPERTY(EditDefaultsOnly, Category="Table Property")
	int32 ActiveStar;
	UPROPERTY(EditDefaultsOnly, Category="Table Property")
	UTexture2D* SlotBackgroundImage;
	
public:
	virtual void SetItemProperties(EItemState CurrentState);
	void DecremenetAmmo();
	bool IsMagazineFull();

	void SetWeaponRarityTableProperties();
	
	// Setters
	void ReloadAmmo(int32 Ammo);
	FORCEINLINE void SetbIsMovingMagazine(bool bIsMoving) { bIsMovingMagazine = bIsMoving; }
	FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index; }	
	// Getters
	const USkeletalMeshSocket* GetBarrelSocket() const;
	FTransform GetBarrelSocketTransform() const;
	FORCEINLINE int32 GetCurrentAmmo() const { return CurrentAmmoAmount; }
	FORCEINLINE int32 GetMagazineCapacity() const { return MagazineCapacity; }
	FORCEINLINE int32 GetSlotIndex() const { return SlotIndex; }
	FORCEINLINE EWeaponType GetWeaponType() const { return WeaponType; }
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE FName GetReloadingWeaponSection() const { return ReloadingMontageSection; }
	FORCEINLINE FName GetMagazineBoneName() const { return MagazineBoneName; }
	FORCEINLINE bool IsMovingClip() const { return bIsMovingMagazine; }
	FORCEINLINE TObjectPtr<UInformationPopUp> GetInformationWidgetObject() const { return InformationWidgetObject; };
	FORCEINLINE UTexture2D* GetSlotBackgroundImage() const { return SlotBackgroundImage; }
};