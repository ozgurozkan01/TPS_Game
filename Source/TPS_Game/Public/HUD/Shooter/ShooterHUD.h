// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

class AWeapon;
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
	FVector2D CalculateCrosshairLocation(int32 ScreenWidth, int32 ScreenHeight);
	float CalculateCrosshairSpreadValue();
	void SetCrosshairTextures();
private:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture> CrosshairMiddle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture> CrosshairLeft;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture> CrosshairRight;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture> CrosshairBottom;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<UTexture> CrosshairTop;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess))
	int32 CrosshairWidth = 48;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess))
	int32 CrosshairHeight = 48;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Combat, meta=(AllowPrivateAccess))
	int32 CrosshairLocationShifting = 20;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess))
	TObjectPtr<AShooterCharacter> ShooterCharacter;
};
