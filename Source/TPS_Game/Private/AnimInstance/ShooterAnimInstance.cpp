// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/ShooterAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	}
}
