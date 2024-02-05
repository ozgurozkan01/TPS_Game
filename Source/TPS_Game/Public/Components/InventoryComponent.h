// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AmmoType.h"
#include "InventoryComponent.generated.h"

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
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Owner, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AShooterCharacter> OwnerRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Inventory, meta=(AllowPrivateAccess="true"))
	TArray<TObjectPtr<ABaseItem>> Inventory;
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
	FORCEINLINE TArray<TObjectPtr<ABaseItem>> GetInventory() const { return Inventory; }
	int32 GetAmmoCountByWeaponType();
	bool CarryingAmmo();

	void AddElementToInventory(TObjectPtr<ABaseItem> AddedItem);
	void GetPickUpItem(TObjectPtr<ABaseItem> PickedUpItem);

	void SwapWeapon(TObjectPtr<AWeapon> WeaponToSwap);
	void DropWeapon();
	
	/** Ammo Type Functions */
	void InitializeAmmoMap();
	void PickUpAmmo(TObjectPtr<AAmmo> PickedUpAmmo);
	void UpdateAmmoMap(EAmmoType AmmoType, int32 AmmoAmount);
};
