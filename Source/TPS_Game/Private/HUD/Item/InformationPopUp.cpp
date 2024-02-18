// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/Item/InformationPopUp.h"

#include "Character/ShooterCharacter.h"
#include "Components/Image.h"
#include "Components/InventoryComponent.h"
#include "Kismet/GameplayStatics.h"

void UInformationPopUp::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ShooterRef = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	
	if (Star1Image && Star2Image && Star3Image && Star4Image && Star5Image)
	{
		StarImageArray.Add(Star1Image);
		StarImageArray.Add(Star2Image);
		StarImageArray.Add(Star3Image);
		StarImageArray.Add(Star4Image);
		StarImageArray.Add(Star5Image);
	}

	for (auto StartImage : StarImageArray)
	{
		StartImage->SetOpacity(0);
	}

	IconLabelText->Font.Size = 17;
}

void UInformationPopUp::SetRightBoxBackgroundColors(FLinearColor& BrightColor, FLinearColor& DarkColor)
{
	RightBottomBackground->SetColorAndOpacity(DarkColor);
	RightTopBackground->SetColorAndOpacity(BrightColor);
}

void UInformationPopUp::SetItemNameText(FString& ItemName)
{
	ItemNameText->SetText(FText::FromString(ItemName));
}

void UInformationPopUp::SetAmmoAmountText(int8 Amount)
{
	FString AmmoAmountFromInt = FString::FromInt(Amount);
	AmmoAmountText->SetText(FText::FromString(AmmoAmountFromInt));
}

void UInformationPopUp::SetItemLabelText()
{
	if (ShooterRef && ShooterRef->GetInventoryComponent() && ShooterRef->GetInventoryComponent()->IsInventoryFull())
	{
		IconLabelText->Font.Size = 10;
		IconLabelText->SetText(FText::FromString("Swap Weapon"));
	}
		
	else
	{
		IconLabelText->SetText(FText::FromString("Pick Up"));
	}
}

void UInformationPopUp::SetStarsImagesVisibility(int32 ActiveStarAmount)
{
	if (StarImageArray.Num() == 0) { return; }
	
	for (uint8 i = 0; i < ActiveStarAmount; i++)
	{
		StarImageArray[i]->SetOpacity(1);
	}
}
