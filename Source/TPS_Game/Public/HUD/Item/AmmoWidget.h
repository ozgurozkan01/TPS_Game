// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AmmoWidget.generated.h"

class UImage;
class UTextBlock;

UCLASS()
class TPS_GAME_API UAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void SetAmmoIcon(UTexture2D* Icon);
	UFUNCTION(BlueprintCallable)
	void SetAmmoAmount(int32 Amount);

private:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> AmmoIcon;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> AmmoAmount;
};
