// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Item/AmmoWidget.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"

void UAmmoWidget::SetAmmoIcon(UTexture2D* Icon)
{
	AmmoIcon->SetBrushFromTexture(Icon, true);
}

void UAmmoWidget::SetAmmoAmount(int32 Amount)
{
	FString AmountString = FString::FromInt(Amount);
	FText AmounText = FText::FromString(AmountString);
	AmmoAmount->SetText(AmounText);
}