// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/WeaponWidget.h"
#include "Character/ShooterCharacter.h"
#include "Components/InventoryComponent.h"
#include "Item/Weapon.h"
#include "Kismet/GameplayStatics.h"

void UWeaponWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TObjectPtr<APawn> Pawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (Pawn)
	{
		ShooterRef = Cast<AShooterCharacter>(Pawn);
	}

	UpdateCurrentAmmoText();
}

FText UWeaponWidget::UpdateCurrentAmmoText()
{
	if (ShooterRef == nullptr || ShooterRef->GetEquippedWeapon() == nullptr) { return FText(); }

	FString CurrentAmmoString = FString::FromInt(ShooterRef->GetEquippedWeapon()->GetCurrentAmmo());
	FText CurrentAmmoText = FText::FromString(CurrentAmmoString);

	return CurrentAmmoText;
}

FText UWeaponWidget::UpdateMaxAmmoText()
{
	if (ShooterRef == nullptr || ShooterRef->GetInventoryComponent() == nullptr || ShooterRef->GetEquippedWeapon() == nullptr) { return FText(); }

	FString CurrentMaxAmmoString = FString::FromInt(ShooterRef->GetInventoryComponent()->GetAmmoCountByWeaponType());
	FText CurrentMaxAmmoText = FText::FromString(CurrentMaxAmmoString);

	return CurrentMaxAmmoText;
}

FText UWeaponWidget::UpdateWeaponNameText()
{
	if (ShooterRef == nullptr || ShooterRef->GetEquippedWeapon() == nullptr) { return FText(); }

	FString CurrentMaxAmmoString = ShooterRef->GetEquippedWeapon()->GetItemName();
	FText CurrentMaxAmmoText = FText::FromString(CurrentMaxAmmoString);

	return CurrentMaxAmmoText;

}
