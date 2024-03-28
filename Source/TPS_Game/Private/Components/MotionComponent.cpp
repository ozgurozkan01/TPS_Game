// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MotionComponent.h"

#include "Character/ShooterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/CombatComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UMotionComponent::UMotionComponent() :
	// Turn Rates for aiming/not aiming
	HipTurnAndLookUpRates(1.f, 1.f),
	AimingTurnAndLookUpRates(0.5f, 0.5f),
	RunningSpeed(650.f),
	CrouchingSpeed(300.f),
	bIsCrouching(false),
	RunningHalfHeight(88.f),
	CrouchingHalfHeight(50.f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UMotionComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerRef == nullptr)
	{
		OwnerRef = Cast<AShooterCharacter>(GetOwner());
	}
}

void UMotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	SetLookRates();
	UpdateCapsuleHalfHeight(DeltaTime);
	GEngine->AddOnScreenDebugMessage(-11, -1, FColor::Black, FString::Printf(TEXT("%d"), bIsCrouching));
}

void UMotionComponent::Movement(const FVector2D& Value)
{
	if (OwnerRef)
	{
		/** Movement Direction Vector comes from the keyboard */
		const FVector2D MovementDirection = Value;
		/** Controller Rotation which is chanhing by the mouse or etc. */
		const FRotator ControllerRotation = OwnerRef->GetControlRotation();
		/** Yaw component of the controller rotation value. */
		const FRotator YawRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);
		/** Set forward vector by using the rotation matrix. */
		const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		OwnerRef->AddMovementInput(ForwardVector, MovementDirection.Y);
		/** Set right vector by using the rotation matrix. */
		const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		OwnerRef->AddMovementInput(RightVector, MovementDirection.X);	
	}
}

void UMotionComponent::CrouchStart(const bool& Value)
{
	if (OwnerRef)
	{
		SetIsCrouching(Value);
		OwnerRef->GetCharacterMovement()->MaxWalkSpeed = CrouchingSpeed;
		OwnerRef->GetCharacterMovement()->GroundFriction = 5.f;
	}
}

void UMotionComponent::CrouchStop(const bool& Value)
{
	if (OwnerRef)
	{
		SetIsCrouching(Value);
		OwnerRef->GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		OwnerRef->GetCharacterMovement()->GroundFriction = .75f;		
	}
}

void UMotionComponent::LookAround(const FVector2D& LookDirection)
{
	if (OwnerRef)
	{
		OwnerRef->AddControllerYawInput(LookDirection.X * BaseTurnAndLoopUpRates.X);
		OwnerRef->AddControllerPitchInput(LookDirection.Y * BaseTurnAndLoopUpRates.Y);
	}
}

void UMotionComponent::SetLookRates()
{
	if (OwnerRef && OwnerRef->GetCombatComponent())
	{
		OwnerRef->GetCombatComponent()->GetIsAiming() ? BaseTurnAndLoopUpRates = AimingTurnAndLookUpRates : BaseTurnAndLoopUpRates = HipTurnAndLookUpRates;
	}
}

void UMotionComponent::UpdateCapsuleHalfHeight(float DeltaTime)
{
	bIsCrouching ? TargetHalfHeight = CrouchingHalfHeight : TargetHalfHeight = RunningHalfHeight;

	if (OwnerRef)
	{
		const float InterpHalfHeight { FMath::FInterpTo(
	OwnerRef->GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
	TargetHalfHeight,
	DeltaTime,
	20.f)};

		const float DeltaCapsuleHalfHeight { InterpHalfHeight - OwnerRef->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
		const FVector MeshOffset { 0.f, 0.f, -DeltaCapsuleHalfHeight};
		OwnerRef->GetMesh()->AddLocalOffset(MeshOffset);
	
		OwnerRef->GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
	}
}