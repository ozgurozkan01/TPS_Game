// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/AmmoWidget.h"

#include "Character/ShooterCharacter.h"
#include "Components/TextBlock.h"
#include "Item/Weapon.h"
#include "Kismet/GameplayStatics.h"

void UAmmoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TObjectPtr<APawn> Pawn = UGameplayStatics::GetPlayerPawn(this, 0);

	if (Pawn)
	{
		ShooterRef = Cast<AShooterCharacter>(Pawn);
	}

	UpdateCurrentAmmoText();
}

FText UAmmoWidget::UpdateCurrentAmmoText()
{
	if (ShooterRef == nullptr || ShooterRef->GetEquippedWeapon() == nullptr) { return FText(); }

	FString CurrentAmmoInt = FString::FromInt(ShooterRef->GetEquippedWeapon()->GetCurrentAmmo());
	FText CurrentAmmo = FText::FromString(CurrentAmmoInt);

	return CurrentAmmo;
}

void UAmmoWidget::UpdateMaxAmmoText()
{

}
