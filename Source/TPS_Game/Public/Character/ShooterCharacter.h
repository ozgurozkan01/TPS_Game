// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ShooterCharacter.generated.h"

struct FInputActionValue;
class UInputAction;
class UInputMappingContext;
class UCameraComponent;
class USpringArmComponent;
class USoundCue;
class UAnimMontage;
class UParticleSystem;
class UParticleSystemComponent;

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

	UFUNCTION(BlueprintCallable)
	void PlayRightBarrelMuzzleFlash();
	UFUNCTION(BlueprintCallable)
	void PlayLeftBarrelMuzzleFlash();
	UFUNCTION(BlueprintCallable)
	void PlayFireSoundCue();
	
private:

	/** Input Functions */
	UFUNCTION()
	void Movement(const FInputActionValue& Value);
	UFUNCTION()
	void LookAround(const FInputActionValue& Value);
	UFUNCTION()
	void Fire(const FInputActionValue& Value);

	/** Combat Functions*/
	FTransform GetGunBarrelSocketTransform(FName GunBarrelSocket);
	bool IsConvertedScreenToWorld(FVector& CrosshairWorldPosition,FVector& CrosshairWorldDirection);
	void LineTraceFromTheScreen(const FVector& CrosshairWorldPosition, const FVector& CrosshairWorldDirection, FVector& BeamEndPoint);
	void LineTraceFromTheGunBarrel(const FVector& GunSocketLocation, FVector& BeamEndPoint);
	void PlayGunFireMontage();
	void PlayHitParticle(const FVector& HitLocation);
	void PlayBeamParticle(const FTransform& Start, const FVector& End);
	void Shoot();
	
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
public:
	/** Getter Functıons */
	FORCEINLINE TObjectPtr<USpringArmComponent> GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE TObjectPtr<UCameraComponent> GetFollowCamera() const {return FollowCamera; }
	
};