// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/ShooterPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "HUD/Shooter/HUDOverlay.h"

AShooterPlayerController::AShooterPlayerController()
{
}

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HUDOverlayClass)
	{
		HUDOverlay = CreateWidget<UHUDOverlay>(this, HUDOverlayClass);

		if (HUDOverlay)
		{
			HUDOverlay->AddToViewport();
			HUDOverlay->SetVisibility(ESlateVisibility::Visible);
		}
	}
}
