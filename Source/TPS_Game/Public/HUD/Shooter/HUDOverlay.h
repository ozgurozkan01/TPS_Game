// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HUDOverlay.generated.h"

class UInventoryWidget;
/**
 * 
 */
UCLASS()
class TPS_GAME_API UHUDOverlay : public UUserWidget
{
	GENERATED_BODY()
public:
	FORCEINLINE TObjectPtr<UInventoryWidget> GetInventoryWidget() const { return InventoryWidget; }
private:

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UInventoryWidget> InventoryWidget;
};
