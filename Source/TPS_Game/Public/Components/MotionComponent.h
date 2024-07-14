// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Async/IAsyncTask.h"
#include "Components/ActorComponent.h"
#include "MotionComponent.generated.h"


class AShooterCharacter;
struct FInputActionValue;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_GAME_API UMotionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMotionComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"), meta=(ClampMin  = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	FVector2D BaseTurnAndLoopUpRates;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"), meta=(ClampMin  = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	FVector2D HipTurnAndLookUpRates;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"), meta=(ClampMin  = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	FVector2D AimingTurnAndLookUpRates;

	TObjectPtr<AShooterCharacter> OwnerRef;
	
	float RunningSpeed;
	float CrouchingSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	float RunningHalfHeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	float CrouchingHalfHeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	float TargetHalfHeight;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Capsule", meta=(AllowPrivateAccess = "true"))
	float SlidingHalfHeight;;
	
	bool bIsCrouching;
	bool bSlidingOnGround;
public:	

	void Slide();
	UFUNCTION(BlueprintCallable)
	bool IsMovingForward();
	UFUNCTION(BlueprintCallable)
	bool CanSlide();

	void Movement(const FVector2D& Value);
	void LookAround(const FVector2D& Value);
	void CrouchStart(const bool& Value);
	void CrouchStop(const bool& Value);
	void SetLookRates();
	void UpdateCapsuleHalfHeight(float DeltaTime);
	// Getters
	FORCEINLINE float GetRunningHalfHeight() const { return RunningHalfHeight; }
	FORCEINLINE float GetRunningSpeed() const { return RunningSpeed; }
	FORCEINLINE float GetCrouchingSpeed() const { return CrouchingSpeed; }
	FORCEINLINE bool GetIsCrouching() const { return bIsCrouching; }
	FORCEINLINE void SetIsCrouching(bool bCrouching) { bIsCrouching = bCrouching; }
	FORCEINLINE bool IsSlidingOnGround() const { return bSlidingOnGround; }
	FORCEINLINE void SetIsSlidingOnGround(bool CanSlide) { bSlidingOnGround = CanSlide; }
};
