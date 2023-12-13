// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

class UTexture2D;
class AShooterCharacter;

UCLASS()
class TPS_GAME_API AShooterHUD : public AHUD
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void DrawCrosshair(int32 ScreenWidth, int32 ScreenHeight);

private:

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture> CrosshairLeft;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture> CrosshairRight;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture> CrosshairBottom;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture> CrosshairTop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess))
	int32 CrosshairWidth = 32;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess))
	int32 CrosshairHeight = 32;

	TObjectPtr<AShooterCharacter> ShooterCharacter;
};
