// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

class UTextBlock;
class AShooterCharacter;

UCLASS()
class TPS_GAME_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;
public:
	UFUNCTION(BlueprintCallable)
	FText UpdateCurrentAmmoText();
	UFUNCTION(BlueprintCallable)
	FText UpdateMaxAmmoText();
private:

	TObjectPtr<AShooterCharacter> ShooterRef;
};
