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
	RootYawOffset(0.f)
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
		/** Set the speed of character */
		FVector CurrentVelocity = ShooterCharacter->GetCharacterMovement()->Velocity;
		CurrentVelocity.Z = 0.f;
		Speed = CurrentVelocity.Size();

		/** Whether or not the character is in air?*/
		bIsInAir = ShooterCharacter->GetCharacterMovement()->IsFalling();
		
		/** Whether or not the character is accelerating?*/
		if (ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f)
		{
			bIsAccelerating = true;
		}

		else
		{
			bIsAccelerating = false;
		}

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
	//UE_LOG(LogTemp, Warning, TEXT("Aim Rotation Yaw : %f"), AimRotation.Yaw);
	FRotator MovementRotation = GetMovementRotation();
	//UE_LOG(LogTemp, Warning, TEXT("Movement Rotation Yaw : %f"), MovementRotation.Yaw);
	if (ShooterCharacter->GetCharacterMovement()->Velocity.Size() > 0.f)
	{
		LastFrameMovementOffsetYaw = MovementOffsetYaw;
	}
	
	MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
	//UE_LOG(LogTemp, Warning, TEXT("Movement Offset Yaw : %f"), MovementOffsetYaw);
}

void UShooterAnimInstance::TurnInPlace()
{
	if (ShooterCharacter == nullptr) { return; }

	CharacterLastFrameYaw = CharacterCurrentYaw;
	CharacterCurrentYaw = ShooterCharacter->GetActorRotation().Yaw;
	/** Delta yaw value which is just one frame */
	const float DeltaYaw = CharacterCurrentYaw - CharacterLastFrameYaw;

	/** Offset Value to provide root bone yaw rotation does not change (be used in AnimBP to rotate root bone) */ 
	RootYawOffset -= DeltaYaw;
}
