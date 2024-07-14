// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/MotionComponent.h"

#include "Character/ShooterCharacter.h"
#include "Components/AnimatorComponent.h"
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
	bSlidingOnGround(false),
	RunningHalfHeight(88.f),
	CrouchingHalfHeight(50.f),
	SlidingHalfHeight(50)
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
	//GEngine->AddOnScreenDebugMessage(-11, -1, FColor::Black, FString::Printf(TEXT("%d"), bIsCrouching));
	//OwnerRef->GetCapsuleComponent()->SetWorldRotation(FRotator(0, OwnerRef->GetCapsuleComponent()->GetComponentRotation().Yaw , 0));
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
	if (IsSlidingOnGround())
	{
		return;
	}
	
	/*bIsCrouching ? TargetHalfHeight = CrouchingHalfHeight : TargetHalfHeight = RunningHalfHeight;

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
	}*/
}

void UMotionComponent::Slide()
{
	if (!OwnerRef || !OwnerRef->GetCapsuleComponent() || !OwnerRef->GetAnimatorComponent()) { return;}

	OwnerRef->GetCapsuleComponent()->SetCapsuleHalfHeight(SlidingHalfHeight);
	bSlidingOnGround = false;
	OwnerRef->GetMesh()->SetWorldLocation(OwnerRef->GetMesh()->GetComponentLocation() + FVector3d(0, 0, 35));
	OwnerRef->GetAnimatorComponent()->PlaySlidingMontage();
}

bool UMotionComponent::IsMovingForward()
{
	if (!OwnerRef) { return false;}
	
	FVector ForwardVector = OwnerRef->GetActorForwardVector();
	FVector Velocity = OwnerRef->GetVelocity();
	// Normalize the velocity vector
	FVector Direction = Velocity.GetSafeNormal();
	// Calculate the dot product between the forward vector and the direction of movement
	float DotProduct = FVector::DotProduct(ForwardVector, Direction);
	// Calculate the angle in radians between the forward vector and the velocity direction
	float AngleRadians = FMath::Acos(DotProduct);
	// Convert the angle from radians to degrees
	float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);

	UE_LOG(LogTemp, Warning, TEXT("%f"), AngleDegrees);
	
	return AngleDegrees < 5;
}

bool UMotionComponent::CanSlide()
{
	if (!OwnerRef) { return false; }

	return IsMovingForward() &&
		!IsSlidingOnGround() &&
		(OwnerRef->GetCharacterMovement()->Velocity.Length() > 250) &&
		!OwnerRef->GetCharacterMovement()->IsFalling() &&
		!OwnerRef->GetCharacterMovement()->IsCrouching() &&
		!OwnerRef->IsAimingButtonPressed() &&
		!OwnerRef->IsFireButtonPressed();
}