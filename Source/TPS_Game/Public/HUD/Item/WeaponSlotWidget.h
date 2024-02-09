// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponSlotWidget.generated.h"

enum class EItemRarity : uint8;
enum class EWeaponType : uint8;
class AWeapon;
class UTextBlock;
class UImage;
class AShooterCharacter;

UCLASS()
class TPS_GAME_API UWeaponSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// If slot is avaliable for new item the -> false
	// Otherwise -> true
	UFUNCTION(BlueprintCallable)
	UTexture2D* GetBackgrounImage();
	UFUNCTION(BlueprintCallable)
	UTexture2D* GetAmmoIcon();
	UFUNCTION(BlueprintCallable)
	UTexture2D* GetWeaponIcon();
	UFUNCTION(BlueprintCallable)
	FText GetAmmoText();
	
	/*void EmptySlot();
	void FillSlot();*/
	
	FORCEINLINE void SetSlotIndex(int32 Index) { SlotIndex = Index; }
	FORCEINLINE void SetIsSlotAvailable(bool IsAvailable) { bIsSlotAvailable = IsAvailable; }
private:
	virtual void NativeOnInitialized() override;
	bool IsWeaponInSlot();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Shooter, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AShooterCharacter> ShooterRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Shooter, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AWeapon> WeaponRef;	
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> BackgroundImage;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> AmmoIcon;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> WeaponIcon;
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> AmmoText;

	int32 SlotIndex = 0;
	bool bIsSlotAvailable = true;
	
	UPROPERTY(EditDefaultsOnly, Category=Inventory, meta=(AllowPrivateAccess=true))
	TMap<EItemRarity, TObjectPtr<UTexture2D>> BackgroundImageMap;
	UPROPERTY(EditDefaultsOnly, Category=Inventory, meta=(AllowPrivateAccess=true))
	TMap<EWeaponType, TObjectPtr<UTexture2D>> WeaponIconMap;
	UPROPERTY(EditDefaultsOnly, Category=Inventory, meta=(AllowPrivateAccess=true))
	TMap<EWeaponType, TObjectPtr<UTexture2D>> AmmoIconMap;
};