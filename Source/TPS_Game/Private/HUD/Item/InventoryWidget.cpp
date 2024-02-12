// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Item/InventoryWidget.h"

#include "Character/ShooterCharacter.h"
#include "HUD/Item/WeaponSlotWidget.h"
#include "Kismet/GameplayStatics.h"

void UInventoryWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ShooterRef == nullptr)
	{
		ShooterRef = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	}	
	
	if (CurrentWeaponSlot)
	{
		CurrentWeaponSlot->SetSlotIndex(0);
	}
	if (WeaponSlot1)
	{
		WeaponSlot1->SetSlotIndex(1);
	}
	if (WeaponSlot2)
	{
		WeaponSlot2->SetSlotIndex(2);
	}
	if (WeaponSlot3)
	{
		WeaponSlot3->SetSlotIndex(3);
	}
	if (WeaponSlot4)
	{
		WeaponSlot4->SetSlotIndex(4);
	}
	if (WeaponSlot5)
	{
		WeaponSlot5->SetSlotIndex(5);
	}

	WeaponSlotMap.Add(0, CurrentWeaponSlot);
	WeaponSlotMap.Add(1, WeaponSlot1);
	WeaponSlotMap.Add(2, WeaponSlot2);
	WeaponSlotMap.Add(3, WeaponSlot3);
	WeaponSlotMap.Add(4, WeaponSlot4);
	WeaponSlotMap.Add(5, WeaponSlot5);

}

void UInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ShooterRef)
	{
		ShooterRef->GetEquipItemDelegate().AddDynamic(this, &UInventoryWidget::EquipItemEvent);
	}	
}

void UInventoryWidget::EquipItemEvent(int32 CurrentSlotIndex, int32 NewSlotIndex)
{
	
}