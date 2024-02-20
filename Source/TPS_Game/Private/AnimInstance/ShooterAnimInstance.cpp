// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/ShooterAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "Components/CombatComponent.h"
#include "Components/MotionComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UShooterAnimInstance::UShooterAnimInstance() :
	Speed(0.f),
	MovementOffsetYaw(0.f),
	LastFrameMovementOffsetYaw(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	bIsAiming(false),
	bIsCrouching(false),
	bIsReloading(false),
	TIP_CharacterCurrentYaw(0.f),
	TIP_CharacterLastFrameYaw(0.f),
	RootYawOffset(0.f),
	CurrentDistanceCurve(0.f),
	LastFrameDistanceCurve(0.f),
	AimOffsetPitch(0.f),
	OffsetState(EOffsetState::EOS_Idle),
	LeanCharacterRotation(FRotator::ZeroRotator),
	LeanCharacterRotationLastFrame(FRotator::ZeroRotator),
	LeanDeltaYawOffset(0.f),
	bIsTurningInPlace(false),
	RecoilWeight(0.f)
{
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}

void UShooterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	UpdateAnimationTransitionVariables();
}

FRotator UShooterAnimInstance::GetAimRotation()
{
	FRotator AimRotation = ShooterCharacter->GetBaseAimRotation();
	return AimRotation;
}

FRotator UShooterAnimInstance::GetMovementRotation()
{
	FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(ShooterCharacter->GetCharacterMovement()->Velocity);
	return MovementRotation;
}

void UShooterAnimInstance::SetMovementOffsetYaw()
{
	FRotator AimRotation = GetAimRotation();
	FRotator MovementRotation = GetMovementRotation();
	if (ShooterCharacter->GetCharacterMovement() && ShooterCharacter->GetCharacterMovement()->Velocity.Size() > 0.f)
	{
		LastFrameMovementOffsetYaw = MovementOffsetYaw;
	}
	MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
}

void UShooterAnimInstance::TurnInPlace()
{
	AimOffsetPitch = ShooterCharacter->GetBaseAimRotation().Pitch;
	
	 if (Speed > 0 || bIsInAir)
	{
		RootYawOffset = FMath::FInterpTo(RootYawOffset, 0.f, GetDeltaSeconds(), 15.f);
		TIP_CharacterCurrentYaw = ShooterCharacter->GetActorRotation().Yaw;
		TIP_CharacterLastFrameYaw = TIP_CharacterCurrentYaw;
		LastFrameDistanceCurve = 0.f;
		CurrentDistanceCurve = 0.f;
	} 

	else
	{
		TIP_CharacterLastFrameYaw = TIP_CharacterCurrentYaw;
		TIP_CharacterCurrentYaw = ShooterCharacter->GetActorRotation().Yaw;
		/** Delta yaw value which is just one frame */
		const float TIP_DeltaYaw = TIP_CharacterCurrentYaw - TIP_CharacterLastFrameYaw;

		/** Offset Value to provide root bone yaw rotation does not change (be used in AnimBP to rotate root bone)
		 * This value is clamped between [180, -180] */ 
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - TIP_DeltaYaw);

		/** Returns the key value of the current animation frame
		 * Turning is meta data curve, for this reason, it might be just 0 (not animated) or 1 (animated).
		 * If the FName using as parameter is normal curve, then returns the value which corresponds to the frame */
		const float IsTurning { GetCurveValue(FName(TEXT("Turning"))) };

		// Turning
		if (IsTurning > 0)
		{
			bIsTurningInPlace = true;
			LastFrameDistanceCurve = CurrentDistanceCurve;
			CurrentDistanceCurve = GetCurveValue(FName(TEXT("DistanceCurve")));
			/** Value difference between 2 frame */
			const float DeltaRotationCurve { CurrentDistanceCurve - LastFrameDistanceCurve };

			/** RootYawOffset > 0 -> Turning Left
			 *	RootYawOffset < 0 -> Turning Right */
			RootYawOffset > 0 ? RootYawOffset -= DeltaRotationCurve : RootYawOffset += DeltaRotationCurve;
			
			const float AbsRootYawOffset { FMath::Abs(RootYawOffset) };

			/** If the character turned to much ? */
			if (AbsRootYawOffset > 90.f)
			{
				/** Extra rotation that was taken while rotating */
				const float YawExcess { AbsRootYawOffset - 90.f} ;
				/** Do not take extra rotation, we need to subtract or add YaxExcess value to RootYawOffset
				 *  By doing this, we are turning closer to 90 degrees. Not more */
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
		else
		{
			bIsTurningInPlace = false;
		}
	}

	UpdateRecoilWeight();
}

void UShooterAnimInstance::Lean()
{
	if (ShooterCharacter == nullptr) { return; }

	LeanCharacterRotationLastFrame = LeanCharacterRotation;
	LeanCharacterRotation = ShooterCharacter->GetActorRotation();
	FRotator DeltaRotation { UKismetMathLibrary::NormalizedDeltaRotator(LeanCharacterRotation, LeanCharacterRotationLastFrame) };
	/** Division is for making the target more bigger. Because we want to lean even in small rotations. */
	const double Target { DeltaRotation.Yaw / GetDeltaSeconds() };
	/** Smooth transition */
	const double Interp { FMath::FInterpTo(LeanDeltaYawOffset, Target, GetDeltaSeconds(), 6.f)};
	/** Set the offset range */
	LeanDeltaYawOffset = FMath::Clamp(Interp, -90.f, 90.f);
}

void UShooterAnimInstance::UpdateOffsetState()
{
	if (bIsReloading) { OffsetState = EOffsetState::EOS_Reloading; return;}
	if (bIsInAir) { OffsetState = EOffsetState::EOS_InAir; return; }
	if (bIsAiming) { OffsetState = EOffsetState::EOS_Aiming; return; }

	OffsetState = EOffsetState::EOS_Idle;
}

void UShooterAnimInstance::UpdateAnimationTransitionVariables()
{
	if (ShooterCharacter == nullptr) { return; }

	if (ShooterCharacter->GetEquippedWeapon())
	{
		ShooterWeaponType = ShooterCharacter->GetEquippedWeapon()->GetWeaponType();
	}
	
	if (ShooterCharacter->GetCharacterMovement())
	{
		FVector CurrentVelocity = ShooterCharacter->GetCharacterMovement()->Velocity;
		CurrentVelocity.Z = 0.f;
		Speed = CurrentVelocity.Size();
		/** Whether or not the character is in air?*/
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();
		/** Whether or not the character is accelerating?*/
		ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? bIsAccelerating = true : bIsAccelerating = false;
	}

	if (ShooterCharacter->GetCombatComponent())
	{
		bIsReloading = ShooterCharacter->GetCombatComponent()->GetCombatState() == ECombatState::ECS_Reloading;
		bIsAiming = ShooterCharacter->GetCombatComponent()->GetIsAiming();
		bIsEquipping = ShooterCharacter->GetCombatComponent()->GetCombatState() == ECombatState::ECS_Equipping;
		bShouldUseFABRIK =	ShooterCharacter->GetCombatComponent()->GetCombatState() == ECombatState::ECS_Unoccupied ||
							ShooterCharacter->GetCombatComponent()->GetCombatState() == ECombatState::ECS_FireTimerInProgress;
	}

	if (ShooterCharacter->GetMotionComponent())
	{
		bIsCrouching = ShooterCharacter->GetMotionComponent()->GetIsCrouching();
	}
	
	SetMovementOffsetYaw();
	UpdateOffsetState();
	TurnInPlace();
	Lean();
}

void UShooterAnimInstance::UpdateRecoilWeight()
{
	if (bIsTurningInPlace)
	{
		if (bIsReloading || bIsEquipping) { RecoilWeight = 1.f; }
		else { RecoilWeight = 0.f; }
	}
	else
	{
		if (bIsCrouching)
		{
			if (bIsReloading || bIsEquipping) { RecoilWeight = 1.f; }
			else { RecoilWeight = 0.1f; }
		}

		else
		{
			if (bIsAiming || bIsReloading || bIsEquipping) { RecoilWeight = 1.f; }
			else { RecoilWeight = 0.5f; }			
		}
	}
}