// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Item/WeaponSlotWidget.h"
#include "Character/ShooterCharacter.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/InventoryComponent.h"
#include "Item/Weapon.h"
#include "Kismet/GameplayStatics.h"

void UWeaponSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (ShooterRef == nullptr)
	{
		ShooterRef = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	}

	HighlightBorder->SetVisibility(ESlateVisibility::Hidden);
	ArrowIcon->SetVisibility(ESlateVisibility::Hidden);
}

bool UWeaponSlotWidget::IsWeaponInSlot()
{
	bool bWeaponInSlot = false;
	if (ShooterRef && ShooterRef->GetInventoryComponent())
	{
		if (ShooterRef->GetInventoryComponent()->GetInventory().IsValidIndex(SlotIndex))
		{
			TObjectPtr<AWeapon> WeaponInSlot = Cast<AWeapon>(ShooterRef->GetInventoryComponent()->GetInventory()[SlotIndex]);
			if (WeaponInSlot)
			{
				WeaponRef = WeaponInSlot;
				bWeaponInSlot = true;
			}
		}
	}
	return bWeaponInSlot;
}

UTexture2D* UWeaponSlotWidget::GetBackgrounImage()
{
	if (IsWeaponInSlot() && WeaponRef && WeaponRef->GetSlotBackgroundImage())
	{
		 return WeaponRef->GetSlotBackgroundImage();
	}

	return nullptr;
}

UTexture2D* UWeaponSlotWidget::GetAmmoIcon()
{
	if (IsWeaponInSlot() && WeaponRef)
	{
		return AmmoIconMap[WeaponRef->GetWeaponType()];
	}

	return nullptr;
}

UTexture2D* UWeaponSlotWidget::GetWeaponIcon()
{
	if (IsWeaponInSlot() && WeaponRef)
	{
		return WeaponIconMap[WeaponRef->GetWeaponType()];
	}

	return nullptr;
}

FText UWeaponSlotWidget::GetAmmoText()
{
	if (IsWeaponInSlot() && WeaponRef)
	{
		FString AmmoString = FString::FromInt(WeaponRef->GetCurrentAmmo());
		return FText::FromString(AmmoString);
	}

	return FText();
}

void UWeaponSlotWidget::PlayHightlightAnimation()
{
	HighlightBorder->SetVisibility(ESlateVisibility::Visible);
	ArrowIcon->SetVisibility(ESlateVisibility::Visible);
	PlayAnimation(HighlightAnimation, 0 , 0);
}

void UWeaponSlotWidget::StopHightlightAnimation()
{
	HighlightBorder->SetVisibility(ESlateVisibility::Hidden);
	ArrowIcon->SetVisibility(ESlateVisibility::Hidden);
	StopAnimation(HighlightAnimation);
}
