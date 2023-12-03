// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/ShooterHUD.h"

// It is called in the BP with ReceiveDrawHUD event
void AShooterHUD::DrawCrosshair(int32 ScreenWidth, int32 ScreenHeight)
{
	int32 CenterX = ScreenWidth / 2;
	int32 CenterY = ScreenHeight / 2;

	FVector2D ScreenCenter {FVector2D(CenterX, CenterY)};

	FVector2D CrosshairLocation {FVector2D(ScreenCenter.X - (CrosshairWidth / 2), ScreenCenter.Y - (CrosshairHeight / 2))};

	CrosshairLocation.Y -= 50;
	
	DrawTexture(Crosshair, CrosshairLocation.X, CrosshairLocation.Y, CrosshairWidth, CrosshairHeight, 0, 0, 1, 1);
}
