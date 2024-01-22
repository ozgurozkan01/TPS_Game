// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AmmoType.h"
#include "ShooterCharacter.generated.h"

class AWeapon;
class ABaseItem;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class USoundCue;
class UAnimMontage;
class UParticleSystem;
class UParticleSystemComponent;

UENUM(BlueprintType)
enum class ECombatState
{
	ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
	ECS_FireTimerInProgress UMETA(DisplayName = "FireTimerInProgress"),
	ECS_Reloading UMETA(DisplayName = "Reloading"),

	ECS_MAX UMETA(DisplayName = "Default"),
};

UCLASS()
class TPS_GAME_API AShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AShooterCharacter();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Jump() override;
	
	/** Update Overlapped item count and change the bShouldTraceForItems value accordingly */
	void IncrementOverlappedItemCount(int8 Amount);
	/** Camera Interp Location */
	FVector GetCameraInterpLocation();
	void GetPickUpItem(TObjectPtr<ABaseItem> PickedUpItem);
	/** Ammo Type Functions */
	void InitializeAmmoMap();

	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	UFUNCTION(BlueprintCallable)
	void GrabMagazine();
	UFUNCTION(BlueprintCallable)
	void ReplaceMagazine();
	
private:

	/** Input Functions */
	UFUNCTION()
	void Movement(const FInputActionValue& Value);
	UFUNCTION()
	void LookAround(const FInputActionValue& Value);
	UFUNCTION()
	void Fire();
	UFUNCTION()
	void OpenScope(const FInputActionValue& Value);
	UFUNCTION()
	void CloseScope(const FInputActionValue& Value);
	UFUNCTION()
	void SelectButtonPressed(const FInputActionValue& Value);
	UFUNCTION()
	void ReloadButtonPressed(const FInputActionValue& Value); // This function is for when Press R key then Reload weapon.
	UFUNCTION()
	void CrouchingButtonPressed(const FInputActionValue& Value);
	UFUNCTION()
	void CrouchingButtonReleased(const FInputActionValue& Value);
	/** Combat Functions*/
	bool IsConvertedScreenToWorld(FVector& CrosshairWorldPosition, FVector& CrosshairWorldDirection);
	void LineTraceFromTheScreen(const FVector& CrosshairWorldPosition, const FVector& CrosshairWorldDirection, FVector& BeamEndPoint);
	void LineTraceFromTheGunBarrel(const FVector& GunSocketLocation, FVector& BeamEndPoint);
	void LineTraceForInformationPopUp();
	void PlayGunFireMontage();
	void PlayReloadWeaponMontage();
	void PlayHitParticle(const FVector& HitLocation);
	void PlayBeamParticle(const FTransform& Start, const FVector& End);
	void PlayFireSoundCue();
	void PlayBarrelMuzzleFlash();
	void CrosshairStartFireBullet();
	void CrosshairFinishFireBullet();
	/** Aiming Functions */
	float InterpCurrentFOV(float TargetFOV, float DeltaTime);
	void CameraInterpZoom(float DeltaTime);
	void SetLookRates();
	/** Functions that calculate values that affect the crosshair's spread animation */
	void CalculateCrosshairSpreadMultiplier(float DeltaTime);
	float CalculateCrosshairVelocityMultiplier();
	float CalculateCrosshairInAirMultiplier(float DeltaTime);
	float CalculateCrosshairAimingMultiplier(float DeltaTime);
	float CalculateCrosshairFireAimingMultiplier(float DeltaTime);
	/** Automatic Gun Fire Functions */
	void FireButtonPressed(const FInputActionValue& Value);
	void FireButtonReleased(const FInputActionValue& Value);
	void StartFireTimer();
	void AutomaticFireReset();
	/** Weapon */
	TObjectPtr<AWeapon> SpawnDefaultWeapon();
	void EquipWeapon(TObjectPtr<AWeapon> WeaponToEquip);
	void DropWeapon();
	void SwapWeapon(TObjectPtr<AWeapon> WeaponToSwap);
	void ReloadWeapon(); // This function is for reloading weapon automatically when the magazine is empty 
	bool CarryingAmmo();

	void UpdateCapsuleHalfHeight(float DeltaTime);
	void UpdateMeshPositionByHalfHeight();
	
	/** Character Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	
	UPROPERTY(EditDefaultsOnly, Category=Animation)
	TSubclassOf<UAnimInstance> AnimationClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<USoundCue> FireSoundCue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UParticleSystem> MuzzleFlash;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UParticleSystem> HitParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UParticleSystem> SmokeBeamParticle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> GunFireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> ReloadMontage;
	
	/** Item Holder to control the widget visibility */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Item, meta=(AllowPrivateAccess="true"))
	TObjectPtr<ABaseItem> HeldItem;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Item, meta=(AllowPrivateAccess="true"))
	TObjectPtr<ABaseItem> TraceHitItem;
	
	float CameraDefaultFOV;
	float CameraZoomedFOV;
	float CameraCurrentFOV;
	float CameraZoomInterpSpeed;

	bool bAiming; 

	float BaseTurnRate;
	float BaseLookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"), meta=(ClampMin  = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float HipTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"), meta=(ClampMin  = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float HipLookUpRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"), meta=(ClampMin  = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float AimingTurnRate;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"), meta=(ClampMin  = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float AimingLookUpRate;
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Crosshair, meta=(AllowPrivateAccess="true"))
	float FireBulletDuration;

	/** Default Weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AWeapon> EquippedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;	
	
	bool bFiring;
	FTimerHandle FireBulletHandle;

	/** Automatic Fire Factors*/
	bool bFireButtonPressed;
	float AutomaticFireRate;
	FTimerHandle AutomaticFireHandle;

	/** Trace Control Values */
	bool bShouldTraceForItems;
	int8 OverlappedItemCount;

	/** Camera Interp Destination Values */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Item, meta=(AllowPrivateAccess = "true"))
	float CameraForwardDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Item, meta=(AllowPrivateAccess = "true"))
	float CameraUpDistance;

	/** Speed */
	float RunningSpeed;
	float CrouchingSpeed;
	
	/** Input */
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> ShooterInputMapping;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MovementAction;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> FireAction;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ScopeAction;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> SelectAction;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ReloadAction;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> CrouchingAction;
	
	/** Weapon Ammo Variables */
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category= Item, meta=(AllowPrivateAccess = "true"))
	TMap<EAmmoType, int32> AmmoMap;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= Item, meta=(AllowPrivateAccess = "true"))
	int32 Starting9mmAmmo;
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category= Item, meta=(AllowPrivateAccess = "true"))
	int32 StartingARAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category=State, meta=(AllowPrivateAccess = "true"))
	ECombatState CombatState;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess = "true"))
	FTransform MagazineTransform;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> LeftHandSceneComponent;

	bool bIsCrouching;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	float RunningHalfHeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	float CrouchingHalfHeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	float TargetHalfHeight;
public:
	
	/** Getter Functıons */
	int32 GetAmmoCountByWeaponType();
	FORCEINLINE TObjectPtr<USpringArmComponent> GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE TObjectPtr<UCameraComponent> GetFollowCamera() const {return FollowCamera; }
	FORCEINLINE TObjectPtr<AWeapon> GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool GetIsAiming() const { return bAiming; }
	FORCEINLINE bool GetIsCrouching() const { return bIsCrouching; }
	FORCEINLINE float GetCrosshairSpreadValue() const { return CrosshairSpreadMultiplier; }
	FORCEINLINE float GetCrosshairSpreadMax() const { return CrosshairSpreadMax; };
	FORCEINLINE int8 GetOverlappedItemCount() const { return OverlappedItemCount; }
	FORCEINLINE FTransform GetLeftHandSceneCompTransform() const { return LeftHandSceneComponent->GetComponentTransform(); }
	FORCEINLINE ECombatState GetCombatState() const { return CombatState; }
};