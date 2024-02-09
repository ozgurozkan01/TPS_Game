// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InventoryComponent.h"
#include "Item/Weapon.h"
#include "Character/ShooterCharacter.h"
#include "Components/CombatComponent.h"
#include "Item/Ammo.h"

UInventoryComponent::UInventoryComponent() :
	Starting9mmAmmo(90),
	StartingARAmmo(120)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerRef == nullptr)
	{
		OwnerRef = Cast<AShooterCharacter>(GetOwner());
	}
	
	InitializeAmmoMap();
}

void UInventoryComponent::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

void UInventoryComponent::UpdateAmmoMap(EAmmoType AmmoType, int32 AmmoAmount)
{
	AmmoMap.Add(AmmoType, AmmoAmount);
}

void UInventoryComponent::AddElementToInventory(TObjectPtr<ABaseItem> AddedItem)
{
	GEngine->AddOnScreenDebugMessage(1, 10, FColor::Red, "2");
	Inventory.Add(AddedItem);
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UInventoryComponent::SwapWeapon(TObjectPtr<AWeapon> WeaponToSwap)
{
	if (WeaponToSwap)
	{
		DropWeapon();
		OwnerRef->EquipWeapon(WeaponToSwap);
	}
}

void UInventoryComponent::DropWeapon()
{
	if (OwnerRef && OwnerRef->GetEquippedWeapon())
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		OwnerRef->GetEquippedWeapon()->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
		OwnerRef->GetEquippedWeapon()->SetItemState(EItemState::EIS_Falling);
		OwnerRef->GetEquippedWeapon()->ThrowWeapon();
	}
}

void UInventoryComponent::GetPickUpItem(TObjectPtr<ABaseItem> PickedUpItem)
{
	TObjectPtr<AWeapon> PickedUpWeapon = Cast<AWeapon>(PickedUpItem);
	if (PickedUpWeapon)
	{
		if (Inventory.Num() < InventoryCapacity)
		{
			AddElementToInventory(PickedUpWeapon);
			PickedUpWeapon->SetItemProperties(EItemState::EIS_PickedUp);
		}

		else
		{
			SwapWeapon(PickedUpWeapon);
		}
	}

	TObjectPtr<AAmmo> PickedUpAmmo = Cast<AAmmo>(PickedUpItem);
	if (PickedUpAmmo) { PickUpAmmo(PickedUpAmmo); }
}

void UInventoryComponent::PickUpAmmo(TObjectPtr<AAmmo> PickedUpAmmo)
{
	if (OwnerRef)
	{
		if (AmmoMap.Contains(PickedUpAmmo->GetAmmoType()))
		{
			// Get the existing ammo count in the map correspond to the ammo type
			int32 ExistingAmmoCount = AmmoMap[PickedUpAmmo->GetAmmoType()];
			// Add the picked up ammo count to the existing
			ExistingAmmoCount += PickedUpAmmo->GetAmmoCount();
			// Set the new value to the value corresponding to the ammo type on the map
			AmmoMap[PickedUpAmmo->GetAmmoType()] = ExistingAmmoCount;
		}
		// Autamatic Reloading the weapon when it is empty and get the ammo at the same time
		if (OwnerRef->GetEquippedWeapon() &&
			OwnerRef->GetEquippedWeapon()->GetAmmoType() == PickedUpAmmo->GetAmmoType() &&
			OwnerRef->GetEquippedWeapon()->GetCurrentAmmo() == 0)
		{
			OwnerRef->GetCombatComponent()->ReloadWeapon(); 
		}
		PickedUpAmmo->Destroy();
	}
}

int32 UInventoryComponent::GetAmmoCountByWeaponType()
{
	if (OwnerRef && OwnerRef->GetEquippedWeapon())
	{
		const auto AmmoType = OwnerRef->GetEquippedWeapon()->GetAmmoType();
		
		if (AmmoMap.Contains(AmmoType))
		{
			return AmmoMap[AmmoType];
		}	
	}
	return -1;
}

bool UInventoryComponent::CarryingAmmo()
{
	if (OwnerRef && OwnerRef->GetEquippedWeapon())
	{
		EAmmoType AmmoType = OwnerRef->GetEquippedWeapon()->GetAmmoType();

		if (AmmoMap.Contains(AmmoType))
		{
			/** return true if we have ammo corresponding to the AmmoType */
			return AmmoMap[AmmoType] > 0;
		}
		return false;	
	}
	return false;
}