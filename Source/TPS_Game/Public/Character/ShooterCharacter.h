// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

class UInventoryComponent;
class UEffectPlayerComponent;
class UAnimatorComponent;
class UCombatComponent;
class UMotionComponent;
class UTracerComponent;
class UCrosshairAnimatorComponent;
class AWeapon;
class ABaseItem;
struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FEquipItemDelegate, int32, CurrentSlotIndex, int32, NewSlotIndex);

USTRUCT(BlueprintType)
struct FInterpLocation
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> InterpLocation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 InterpingItemCount;	
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
private:

	/** Input Functions */
	UFUNCTION()
	void Movement(const FInputActionValue& Value);
	UFUNCTION()
	void LookAround(const FInputActionValue& Value);
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
	UFUNCTION()
	void FireButtonPressed(const FInputActionValue& Value);
	UFUNCTION()
	void FireButtonReleased(const FInputActionValue& Value);
	UFUNCTION()
	void KeyOnePressed(const FInputActionValue& Value);
	UFUNCTION()
	void KeyTwoPressed(const FInputActionValue& Value);
	UFUNCTION()
	void KeyThreePressed(const FInputActionValue& Value);
	UFUNCTION()
	void KeyFourPressed(const FInputActionValue& Value);
	UFUNCTION()
	void KeyFivePressed(const FInputActionValue& Value);
	
	void Jump();

	/** Aiming Functions */
	float InterpCurrentFOV(float TargetFOV, float DeltaTime);
	void CameraInterpZoom(float DeltaTime);
	
	/** Weapon */
	TObjectPtr<AWeapon> SpawnDefaultWeapon();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	
	/** Character Components */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMotionComponent> MotionComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attribute", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCrosshairAnimatorComponent> CrosshairAnimatorComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attribute", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UTracerComponent> TracerComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attribute", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> CombatComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attribute", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAnimatorComponent> AnimatorComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attribute", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UEffectPlayerComponent> EffectPlayerComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Attribute", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> InventoryComponent;
	
	float CameraDefaultFOV;
	float CameraZoomedFOV;
	float CameraCurrentFOV;
	float CameraZoomInterpSpeed;

	bool bFireButtonPressed;

	/** Default Weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<AWeapon> EquippedWeapon;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TSubclassOf<AWeapon> DefaultWeaponClass;	
	
	/** Camera Interp Destination Values */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Item, meta=(AllowPrivateAccess = "true"))
	float CameraForwardDistance;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category= Item, meta=(AllowPrivateAccess = "true"))
	float CameraUpDistance;

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
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> F_KeyAction;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> Key_1_Action;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> Key_2_Action;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> Key_3_Action;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> Key_4_Action;
	UPROPERTY(EditDefaultsOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> Key_5_Action;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess = "true"))
	FTransform MagazineTransform;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> LeftHandSceneComponent;
	UPROPERTY(EditDefaultsOnly, Category=Animation)
	TSubclassOf<UAnimInstance> AnimationClass;
	
	/** Scene Components of Camera to interpolate items when they are picked up */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> WeaponInterpTargetComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> ItemInterpTargetComp1;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> ItemInterpTargetComp2;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> ItemInterpTargetComp3;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> ItemInterpTargetComp4;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> ItemInterpTargetComp5;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> ItemInterpTargetComp6;

	TArray<FInterpLocation> ItemInterpLocations;

	FEquipItemDelegate EquipItemDelegate;
public:
	void EquipWeapon(TObjectPtr<AWeapon> WeaponToEquip);
	
	/** Update Overlapped item count and change the bShouldTraceForItems value accordingly */
	/** Camera Interp Location */
	FVector GetCameraInterpLocation();


	UFUNCTION(BlueprintCallable)
	void GrabMagazine();
	UFUNCTION(BlueprintCallable)
	void ReplaceMagazine();
	/** Interpolation Methods */
	void InitializeInterpLocationContainer();
	void UpdateInterpingItemCount(int32 Index, int32 Amount);

	int32 GetInterpLocationIndex();
	
	/** Getter Functıons */
	FInterpLocation GetInterpLocation(int32 Index);
	FORCEINLINE FEquipItemDelegate& GetEquipItemDelegate() { return EquipItemDelegate; }
	FORCEINLINE TObjectPtr<USpringArmComponent> GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE TObjectPtr<UCameraComponent> GetFollowCamera() const {return FollowCamera; }
	FORCEINLINE TObjectPtr<UCrosshairAnimatorComponent> GetCrosshairAnimatorComponent() const { return CrosshairAnimatorComponent; }
	FORCEINLINE TObjectPtr<UTracerComponent> GetTracerComponent() const { return TracerComponent; }
	FORCEINLINE TObjectPtr<UMotionComponent> GetMotionComponent() const { return MotionComponent; }
	FORCEINLINE TObjectPtr<UCombatComponent> GetCombatComponent() const { return CombatComponent; }
	FORCEINLINE TObjectPtr<UAnimatorComponent> GetAnimatorComponent() const { return AnimatorComponent; }
	FORCEINLINE TObjectPtr<UEffectPlayerComponent> GetEffectPlayerComponent() const { return EffectPlayerComponent; }
	FORCEINLINE TObjectPtr<UInventoryComponent> GetInventoryComponent() const { return InventoryComponent; }
	
	FORCEINLINE TObjectPtr<AWeapon> GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE FTransform GetLeftHandSceneCompTransform() const { return LeftHandSceneComponent->GetComponentTransform(); }
	FORCEINLINE bool GetIsFireButtonPressed() const { return bFireButtonPressed; }
};