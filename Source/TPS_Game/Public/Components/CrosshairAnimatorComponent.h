// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CrosshairAnimatorComponent.generated.h"


class AShooterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_GAME_API UCrosshairAnimatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCrosshairAnimatorComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Crosshair, meta=(AllowPrivateAccess="true"))
	float CrosshairSpreadMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Crosshair, meta=(AllowPrivateAccess="true"))
	float CrosshairInAirMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Crosshair, meta=(AllowPrivateAccess="true"))
	float CrosshairAimingMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Crosshair, meta=(AllowPrivateAccess="true"))
	float CrosshairShootingMultiplier;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Crosshair, meta=(AllowPrivateAccess="true"))
	float CrosshairSpreadMax;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Owner, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AShooterCharacter> MainCharacter;
public:
	
	/** Functions that calculate values that affect the crosshair's spread animation */
	void CalculateCrosshairSpreadMultiplier(float DeltaTime);
	float CalculateCrosshairVelocityMultiplier();
	float CalculateCrosshairInAirMultiplier(float DeltaTime);
	float CalculateCrosshairAimingMultiplier(float DeltaTime);
	float CalculateCrosshairFireAimingMultiplier(float DeltaTime);

	FORCEINLINE float GetCrosshairSpreadMultiplier() const { return CrosshairSpreadMultiplier; }
	FORCEINLINE float GetCrosshairSpreadMax() const { return CrosshairSpreadMax; };
};
