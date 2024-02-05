// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponWidget.generated.h"

class UTextBlock;
class AShooterCharacter;

UCLASS()
class TPS_GAME_API UWeaponWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
public:
	UFUNCTION(BlueprintCallable)
	FText UpdateCurrentAmmoText();
	UFUNCTION(BlueprintCallable)
	FText UpdateMaxAmmoText();
	UFUNCTION(BlueprintCallable)
	FText UpdateWeaponNameText();
private:

	TObjectPtr<AShooterCharacter> ShooterRef;
};
