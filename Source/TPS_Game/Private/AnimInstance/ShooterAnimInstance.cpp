// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/ShooterAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

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