// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Item/WeaponSlotWidget.h"

#include "Character/ShooterCharacter.h"
#include "Components/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

void UWeaponSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ShooterRef == nullptr)
	{
		ShooterRef = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	}
}

bool UWeaponSlotWidget::WeaponInSlot(int32 Index)
{
	if (ShooterRef && ShooterRef->GetInventoryComponent())
	{
		return ShooterRef->GetInventoryComponent()->GetInventory().IsValidIndex(Index);
	}
	return false;
}
