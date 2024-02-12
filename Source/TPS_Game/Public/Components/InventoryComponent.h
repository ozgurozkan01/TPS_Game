// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

enum class EAmmoType : uint8;
class UInventoryWidget;
class AShooterCharacter;
class AWeapon;
class ABaseItem;
class AAmmo;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_GAME_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UInventoryComponent();

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Inventory, meta=(AllowPrivateAccess))
	TObjectPtr<UInventoryWidget> InventoryWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Inventory, meta=(AllowPrivateAccess="true"))
	TArray<TObjectPtr<AWeapon>> Inventory;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Owner, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AShooterCharacter> OwnerRef;
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category= Item, meta=(AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;
	/** Weapon Ammo Variables */
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= Item, meta=(AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= Item, meta=(AllowPrivateAccess = "true"))
	int32 StartingARAmmo;
	
	const int32 InventoryCapacity{6};
public:
	FORCEINLINE TMap<EAmmoType, int32> GetAmmoMap() const { return AmmoMap; };
	FORCEINLINE TArray<TObjectPtr<AWeapon>> GetInventory() const { return Inventory; }
	int32 GetAmmoCountByWeaponType();
	bool CarryingAmmo();
 
	void AddElementToInventory(TObjectPtr<AWeapon> AddedItem);
	void GetPickUpItem(TObjectPtr<ABaseItem> PickedUpItem);

	void SwapWeapon(TObjectPtr<AWeapon> WeaponToSwap);
	void DropWeapon();
	void ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex);
	
	/** Ammo Type Functions */
	void InitializeAmmoMap();
	void PickUpAmmo(TObjectPtr<AAmmo> PickedUpAmmo);
	void UpdateAmmoMap(EAmmoType AmmoType, int32 AmmoAmount);
};
