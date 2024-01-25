// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Ammo.generated.h"

/**
 * 
 */
UCLASS()
class TPS_GAME_API AAmmo : public ABaseItem
{
	GENERATED_BODY()
public:
	AAmmo();
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void BeginPlay() override;
private:
	virtual void SetItemProperties(EItemState CurrentState);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Ammo, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> AmmoMesh;
};
