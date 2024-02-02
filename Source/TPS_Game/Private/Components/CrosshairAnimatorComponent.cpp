#include "Components/CrosshairAnimatorComponent.h"
#include "Character/ShooterCharacter.h"
#include "Components/CombatComponent.h"
#include "Components/TracerComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

UCrosshairAnimatorComponent::UCrosshairAnimatorComponent() :
	CrosshairSpreadMultiplier(0.5f),
	CrosshairInAirMultiplier(0.f),
	CrosshairAimingMultiplier(0.f),
	CrosshairShootingMultiplier(0.f),
	CrosshairSpreadMax(16.f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCrosshairAnimatorComponent::BeginPlay()
{
	Super::BeginPlay();
	if (MainCharacter == nullptr)
	{
		MainCharacter = Cast<AShooterCharacter>(GetOwner());
	}
}

void UCrosshairAnimatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	CalculateCrosshairSpreadMultiplier(DeltaTime);
}

void UCrosshairAnimatorComponent::CalculateCrosshairSpreadMultiplier(float DeltaTime)
{
	CrosshairSpreadMultiplier =
		0.5 +
		CalculateCrosshairVelocityMultiplier() +
		CalculateCrosshairInAirMultiplier(DeltaTime) +
		CalculateCrosshairFireAimingMultiplier(DeltaTime) -
		CalculateCrosshairAimingMultiplier(DeltaTime);
}

float UCrosshairAnimatorComponent::CalculateCrosshairVelocityMultiplier()
{
	if (MainCharacter)
	{
		FVector2D WalkSpeedRange {0.f, 600.f};
		FVector2D VelocityMultiplierRange {0.f, 1.f};
		FVector CurrentVelocity = MainCharacter->GetVelocity();
		CurrentVelocity.Z = 0.f;

		/** Clamp the speed value based on output range value and return corresponding percentage accordingly output*/
		return FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, CurrentVelocity.Size());	
	}
	return 0.f;
}

float UCrosshairAnimatorComponent::CalculateCrosshairInAirMultiplier(float DeltaTime)
{
	if (MainCharacter && MainCharacter->GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirMultiplier = FMath::FInterpTo(CrosshairInAirMultiplier, 2.25f, DeltaTime, 6.f); 
	}

	else
	{
		CrosshairInAirMultiplier = FMath::FInterpTo(CrosshairInAirMultiplier, 0, DeltaTime, 10.f); 
	}

	return CrosshairInAirMultiplier;
}

float UCrosshairAnimatorComponent::CalculateCrosshairAimingMultiplier(float DeltaTime)
{
	if (MainCharacter && MainCharacter->GetCombatComponent() && MainCharacter->GetCombatComponent()->GetIsAiming())
	{
		CrosshairAimingMultiplier = FMath::FInterpTo(CrosshairAimingMultiplier, 0.35f, DeltaTime, 6.f); 
	}

	else
	{
		CrosshairAimingMultiplier = FMath::FInterpTo(CrosshairAimingMultiplier, 0, DeltaTime, 10.f); 
	}

	return CrosshairAimingMultiplier;
	
}

float UCrosshairAnimatorComponent::CalculateCrosshairFireAimingMultiplier(float DeltaTime)
{
	if (MainCharacter && MainCharacter->GetTracerComponent() && MainCharacter->GetTracerComponent()->GetIsFiring())
	{
		CrosshairShootingMultiplier = FMath::FInterpTo(CrosshairShootingMultiplier, 0.2f, DeltaTime, 45.f); 
	}

	else
	{
		CrosshairShootingMultiplier = FMath::FInterpTo(CrosshairShootingMultiplier, 0.f, DeltaTime, 45.f); 
	}

	return CrosshairShootingMultiplier;
}
