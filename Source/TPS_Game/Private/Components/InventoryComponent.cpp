// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/InventoryComponent.h"
#include "Item/Weapon.h"
#include "Character/ShooterCharacter.h"
#include "Components/AnimatorComponent.h"
#include "Components/CombatComponent.h"
#include "Controller/ShooterPlayerController.h"
#include "HUD/Shooter/HUDOverlay.h"
#include "Item/Ammo.h"

UInventoryComponent::UInventoryComponent() :
	Starting9mmAmmo(90),
	StartingARAmmo(120),
	HightlightSlotIndex(-1)
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

	if (OwnerRef)
	{
		TObjectPtr<AShooterPlayerController> OwnerController = Cast<AShooterPlayerController>(OwnerRef->GetController());
		if (OwnerController)
		{
			InventoryWidget = OwnerController->GetHUDOverlay()->GetInventoryWidget();
		}
	}
	
	InitializeAmmoMap();
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
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

void UInventoryComponent::SetInventorySlotHightlight(bool bIsHightlight)
{
	if (bIsHightlight)
	{
		HightlightSlotIndex = GetEmptySlotIndex();
		HighlightIconDelegate.Broadcast(HightlightSlotIndex, bIsHightlight);
		return;
	}

	HighlightIconDelegate.Broadcast(HightlightSlotIndex, bIsHightlight);
	HightlightSlotIndex = -1;
}

int32 UInventoryComponent::GetEmptySlotIndex()
{
	for (int i = 0; i < Inventory.Num(); i++)
	{
		if (Inventory[i] == nullptr)
		{
			return i;
		}
	}
	
	if (Inventory.Num() < InventoryCapacity)
	{
		return Inventory.Num();
	}
	
	return -1;
}

void UInventoryComponent::AddElementToInventory(TObjectPtr<AWeapon> AddedItem)
{
	Inventory.Add(AddedItem);
}

void UInventoryComponent::SwapWeapon(TObjectPtr<AWeapon> WeaponToSwap)
{
	if (WeaponToSwap)
	{
		// Does slot index have a weapon ? 
		if (OwnerRef->GetEquippedWeapon() && OwnerRef->GetEquippedWeapon()->GetSlotIndex() <= Inventory.Num() - 1)
		{
			// Change the weapon place in the inventory
			// Equipped weapon -> weapon is used already by character
			// Weapon to swap -> weapon intended to be used by character
			Inventory[OwnerRef->GetEquippedWeapon()->GetSlotIndex()] = WeaponToSwap;
			WeaponToSwap->SetSlotIndex(OwnerRef->GetEquippedWeapon()->GetSlotIndex());
		}
		
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

void UInventoryComponent::ExchangeInventoryItems(int32 CurrentItemIndex, int32 NewItemIndex)
{
	if (NewItemIndex >= Inventory.Num() || CurrentItemIndex == NewItemIndex || OwnerRef == nullptr) { return; }

	if (OwnerRef->GetCombatComponent() && OwnerRef->GetCombatComponent()->GetCombatState() == ECombatState::ECS_Unoccupied)
	{
		auto OldEquippedWeapon = OwnerRef->GetEquippedWeapon();
		auto NewEquippedWeapon = Inventory[NewItemIndex];

		TObjectPtr<AWeapon> WeaponTemp = Inventory[CurrentItemIndex];

		// Change Weapon in slot in inventory
		Inventory[CurrentItemIndex] = Inventory[NewItemIndex];
		Inventory[NewItemIndex] = WeaponTemp;
		// Update the weapon slot value of the weapon has changed its place in the inventory.
		Inventory[CurrentItemIndex]->SetSlotIndex(CurrentItemIndex);
		Inventory[NewItemIndex]->SetSlotIndex(NewItemIndex);
	
		if (NewEquippedWeapon)
		{
			// Update Equipped item as new
			OwnerRef->EquipWeapon(NewEquippedWeapon);
			// Update items states
			OldEquippedWeapon->SetItemProperties(EItemState::EIS_PickedUp);
			NewEquippedWeapon->SetItemProperties(EItemState::EIS_Equipped);
		}

		OwnerRef->GetCombatComponent()->SetCombatState(ECombatState::ECS_Equipping);

		if (OwnerRef->GetAnimatorComponent())
		{
			OwnerRef->GetAnimatorComponent()->PlayEquipWeaponMontage();
		}
		NewEquippedWeapon->PlayExchangeSound(true);
	}
}

void UInventoryComponent::GetPickUpItem(TObjectPtr<ABaseItem> PickedUpItem)
{
	TObjectPtr<AWeapon> PickedUpWeapon = Cast<AWeapon>(PickedUpItem);
	if (PickedUpWeapon)
	{
		if (Inventory.Num() < InventoryCapacity)
		{
			PickedUpWeapon->SetSlotIndex(Inventory.Num());
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