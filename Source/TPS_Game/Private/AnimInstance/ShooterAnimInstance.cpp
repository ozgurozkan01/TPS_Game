// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/ShooterAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

UShooterAnimInstance::UShooterAnimInstance() :
	Speed(0.f),
	MovementOffsetYaw(0.f),
	LastFrameMovementOffsetYaw(0.f),
	bIsInAir(false),
	bIsAccelerating(false),
	bAiming(false),
	CharacterCurrentYaw(0.f),
	CharacterLastFrameYaw(0.f),
	RootYawOffset(0.f),
	CurrentDistanceCurve(0.f),
	LastFrameDistanceCurve(0.f),
	AimOffsetPitch(0.f),
	bIsReloading(false)
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

	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if (ShooterCharacter)
	{
		if (ShooterCharacter->GetCharacterMovement())
		{
			/** Set the speed of character */
			FVector CurrentVelocity = ShooterCharacter->GetCharacterMovement()->Velocity;
			CurrentVelocity.Z = 0.f;
			Speed = CurrentVelocity.Size();

			/** Whether or not the character is in air?*/
			bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();
		
			/** Whether or not the character is accelerating?*/
			ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? bIsAccelerating = true : bIsAccelerating = false;
		}

		bIsReloading = 	ShooterCharacter->GetCombatState() == ECombatState::ECS_Reloading;
		SetMovementOffsetYaw();
		bAiming = ShooterCharacter->GetIsAiming();
	}

	TurnInPlace();
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
	if (ShooterCharacter == nullptr) { return; }
	/*/** If character is moving or falling down, RootYawOffset should be zero #1#
	if (Speed > 0 || bIsInAir)
	{
		RootYawOffset = UKismetMathLibrary::FInterpTo(RootYawOffset, 0.f, GetDeltaSeconds(), 20.f);
		CharacterCurrentYaw = ShooterCharacter->GetActorRotation().Yaw;
		CharacterLastFrameYaw = CharacterCurrentYaw;
		CurrentDistanceCurve = 0.f;
		LastFrameDistanceCurve = 0.f;
	}

	else
	{
		CharacterLastFrameYaw = CharacterCurrentYaw;
		CharacterCurrentYaw = ShooterCharacter->GetActorRotation().Yaw;

		/** Yaw Rotation difference between 2 frame #1#
		const float DeltaYawRotation = { CharacterCurrentYaw - CharacterLastFrameYaw };

		RootYawOffset -= DeltaYawRotation;

		/** Returns the key value of the current animation frame
		 * Turning is meta data curve, for this reason, it might be just 0 (not animated) or 1 (animated).
		 * If the FName using as parameter is normal curve, then returns the value which corresponds to the frame
		#1#
		const float bIsTurning = GetCurveValue(FName(TEXT("Turning")));
		
		if (bIsTurning > 0)
		{
			LastFrameDistanceCurve = CurrentDistanceCurve;
			CurrentDistanceCurve = GetCurveValue(FName(TEXT("DistanceCurve")));

			const float DeltaDistanceCurve { CurrentDistanceCurve - LastFrameDistanceCurve };

			// Is Turning Left ? Then rotate Root Yaw towards the left : Otherwise rotate Root Yaw towards the right
			RootYawOffset > 0 ? RootYawOffset -= DeltaDistanceCurve : RootYawOffset += DeltaDistanceCurve;

			const float AbsRootYawOffset { FMath::Abs(RootYawOffset) };

			if (AbsRootYawOffset > 90.f)
			{
				/** Extra rotation that was taken while rotating #1#
				const float YawExcess {AbsRootYawOffset - 90.f};
				/** Do not take extra rotation, we need to subtract or add YaxExcess value to RootYawOffset
				 *  By doing this, we are turning closer to 90 degrees. Not more #1#
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(2, -1, FColor::Green, FString::Printf(TEXT("Root Offset : %f"), RootYawOffset));
	}*/

	AimOffsetPitch = ShooterCharacter->GetBaseAimRotation().Pitch;
	GEngine->AddOnScreenDebugMessage(3, -1, FColor::Blue, FString::Printf(TEXT("AimOffsetPitch : %f"), AimOffsetPitch));

	
	 if (Speed > 0)
	{
		RootYawOffset = FMath::FInterpTo(RootYawOffset, 0.f, GetDeltaSeconds(), 15.f);
		CharacterCurrentYaw = ShooterCharacter->GetActorRotation().Yaw;
		CharacterLastFrameYaw = CharacterCurrentYaw;
		LastFrameDistanceCurve = 0.f;
		CurrentDistanceCurve = 0.f;
	} 

	else
	{
		CharacterLastFrameYaw = CharacterCurrentYaw;
		CharacterCurrentYaw = ShooterCharacter->GetActorRotation().Yaw;
		/** Delta yaw value which is just one frame */
		const float DeltaYaw = CharacterCurrentYaw - CharacterLastFrameYaw;

		/** Offset Value to provide root bone yaw rotation does not change (be used in AnimBP to rotate root bone)
		 * This value is clamped between [180, -180] */ 
		RootYawOffset = UKismetMathLibrary::NormalizeAxis(RootYawOffset - DeltaYaw);

		/** Returns the key value of the current animation frame
		 * Turning is meta data curve, for this reason, it might be just 0 (not animated) or 1 (animated).
		 * If the FName using as parameter is normal curve, then returns the value which corresponds to the frame
		 */
		const float IsTurning { GetCurveValue(FName(TEXT("Turning"))) };

		// Turning
		if (IsTurning > 0)
		{
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
				UE_LOG(LogTemp, Warning, TEXT("YawExcess : %f"), YawExcess);
				/** Do not take extra rotation, we need to subtract or add YaxExcess value to RootYawOffset
				 *  By doing this, we are turning closer to 90 degrees. Not more */
				RootYawOffset > 0 ? RootYawOffset -= YawExcess : RootYawOffset += YawExcess;
			}
		}
		
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(1, -1, FColor::Blue, FString::Printf(TEXT("Current Yaw : %f"), CharacterCurrentYaw));
			GEngine->AddOnScreenDebugMessage(2, -1, FColor::Green, FString::Printf(TEXT("Root Offset : %f"), RootYawOffset));

		}
	}
}
		