// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ShooterHUD.h"

#include "Character/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"

void AShooterHUD::BeginPlay()
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	}
}

// It is called in the BP with ReceiveDrawHUD event
void AShooterHUD::DrawCrosshair(int32 ScreenWidth, int32 ScreenHeight)
{
	float SpreadValue = CalculateCrosshairSpreadValue();
	FVector2D CrosshairLocation = CalculateCrosshairLocation(ScreenWidth, ScreenHeight);
	
	DrawTexture(CrosshairLeft, CrosshairLocation.X - SpreadValue, CrosshairLocation.Y, CrosshairWidth, CrosshairHeight, 0, 0, 1, 1);
	DrawTexture(CrosshairRight, CrosshairLocation.X + SpreadValue, CrosshairLocation.Y, CrosshairWidth, CrosshairHeight, 0, 0, 1, 1);
	DrawTexture(CrosshairBottom, CrosshairLocation.X, CrosshairLocation.Y + SpreadValue, CrosshairWidth, CrosshairHeight, 0, 0, 1, 1);
	DrawTexture(CrosshairTop, CrosshairLocation.X, CrosshairLocation.Y - SpreadValue, CrosshairWidth, CrosshairHeight, 0, 0, 1, 1);
}

FVector2D AShooterHUD::CalculateCrosshairLocation(int32 ScreenWidth, int32 ScreenHeight)
{
	int32 CenterX = ScreenWidth / 2;
	int32 CenterY = ScreenHeight / 2;

	FVector2D ScreenCenter {FVector2D(CenterX, CenterY)};
	FVector2D CrosshairLocation {FVector2D(ScreenCenter.X - (CrosshairWidth / 2), ScreenCenter.Y - (CrosshairHeight / 2))};

	return CrosshairLocation;
}

float AShooterHUD::CalculateCrosshairSpreadValue()
{
	if (ShooterCharacter)
	{
		return ShooterCharacter->GetCrosshairSpreadValue() * ShooterCharacter->GetCrosshairSpreadMax();
	}

	return 0.f;
}
