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
		InventorySlotArray.Add(CurrentWeaponSlot);
	}
	if (WeaponSlot1)
	{
		WeaponSlot1->SetSlotIndex(1);
		InventorySlotArray.Add(WeaponSlot1);
	}
	if (WeaponSlot2)
	{
		WeaponSlot2->SetSlotIndex(2);
		InventorySlotArray.Add(WeaponSlot2);
	}
	if (WeaponSlot3)
	{
		WeaponSlot3->SetSlotIndex(3);
		InventorySlotArray.Add(WeaponSlot3);
	}
	if (WeaponSlot4)
	{
		WeaponSlot4->SetSlotIndex(4);
		InventorySlotArray.Add(WeaponSlot4);
	}
	if (WeaponSlot5)
	{
		WeaponSlot5->SetSlotIndex(5);
		InventorySlotArray.Add(WeaponSlot5);
	}
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