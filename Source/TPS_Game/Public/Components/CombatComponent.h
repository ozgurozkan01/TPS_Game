// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class AShooterCharacter;
class UShooterAnimInstance;
class USoundCue;

UENUM(BlueprintType)
enum class ECombatState
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),
	ECS_Equipping UMETA(DisplayName = "Equipping"),

	ECS_MAX UMETA(DisplayName = "Default"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_GAME_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()
public:	
	UCombatComponent();
protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY()
	TObjectPtr<AShooterCharacter> OwnerRef;
	
	/** Automatic Fire Factors*/
	float AutomaticFireRate;
	FTimerHandle AutomaticFireHandle;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=State, meta=(AllowPrivateAccess = "true"))
	ECombatState CombatState;

	
	bool bAiming; 
	bool bIsScopeOpen;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ShootingStart();

	void StartAim();
	void StopAim();
	
	void StartFireTimer();
	void AutomaticFireReset();

	void ReloadWeapon(); // This function is for reloading weapon automatically when the magazine is empty 
	void FinishReloading();
	
	FORCEINLINE void SetCombatState(ECombatState CurrentState) { CombatState = CurrentState; }
	FORCEINLINE void SetIsAiming(bool bIsAiming) { bAiming = bIsAiming; } 
	FORCEINLINE void SetIsScopeOpen(bool bIsOpen) { bIsScopeOpen = bIsOpen; } 
	
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
	FORCEINLINE bool GetIsAiming() const { return bAiming; }
	FORCEINLINE bool GetIsScopeOpen() const { return bIsScopeOpen; }
};
