// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ShooterPlayerController.generated.h"

class UUserWidget;

UCLASS()
class TPS_GAME_API AShooterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AShooterPlayerController();
protected:
	virtual void BeginPlay() override;

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Widget, meta=(AllowPrivateAccess = "true"))
	TSubclassOf<UUserWidget> HUDOverlayClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Widget, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UUserWidget> HUDOverlay;
};
