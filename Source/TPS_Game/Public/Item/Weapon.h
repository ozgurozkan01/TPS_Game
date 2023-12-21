// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Weapon.generated.h"

UCLASS()
class TPS_GAME_API AWeapon : public ABaseItem
{
	GENERATED_BODY()

public:
	AWeapon();
	virtual void Tick(float DeltaSeconds) override;
	
protected:
	virtual void BeginPlay() override;

private: // Variables

	UPROPERTY(EditAnywhere, Category="Weapon Properties", meta=(AllowPrivateAccess = "true"))
	uint8 MaxAmmoAmount;

	UPROPERTY(EditAnywhere, Category="Weapon Properties", meta=(AllowPrivateAccess = "true"))
	uint8 CurrentAmmoAmount;
	
public: // Getters and Setters

	const USkeletalMeshSocket* GetBarrelSocket() const;
	FTransform GetBarrelSocketTransform() const;
};
