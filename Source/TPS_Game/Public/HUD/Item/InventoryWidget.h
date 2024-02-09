// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryWidget.generated.h" 

class UWeaponSlotWidget;
class AShooterCharacter;

UCLASS()
class TPS_GAME_API UInventoryWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	/** Delegate Functions */
	UFUNCTION()
	void EquipItemEvent(int32 CurrentSlotIndex, int32 NewSlotIndex);
private:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Shooter, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AShooterCharacter> ShooterRef;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWeaponSlotWidget> CurrentWeaponSlot; 
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWeaponSlotWidget> WeaponSlot1;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWeaponSlotWidget> WeaponSlot2;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWeaponSlotWidget> WeaponSlot3;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWeaponSlotWidget> WeaponSlot4;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UWeaponSlotWidget> WeaponSlot5;

	TArray<TObjectPtr<UWeaponSlotWidget>> InventorySlotArray;
};
