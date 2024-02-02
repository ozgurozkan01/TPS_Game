#include "Components/EffectPlayerComponent.h"
#include "Character/ShooterCharacter.h"
#include "Item/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

UEffectPlayerComponent::UEffectPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UEffectPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerRef)
	{
		OwnerRef = Cast<AShooterCharacter>(GetOwner());
	}
}

void UEffectPlayerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UEffectPlayerComponent::PlayHitParticle(const FVector& HitLocation)
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitLocation);
	}
}

void UEffectPlayerComponent::PlayBeamParticle(const FTransform& Start, const FVector& End)
{
	if (SmokeBeamParticle)
	{
		/** Spawn the Beam Particle and Store in the variable */
		TObjectPtr<UParticleSystemComponent> BeamParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeBeamParticle, Start);
		/** This Particle has the target point and target point represents the end location. If we do not set, the end point is set FVector(0, 0, 0)
		 * To set this variable we need to store it in variable. */
		BeamParticleComponent->SetVectorParameter(FName("Target"), End);
	}
}

void UEffectPlayerComponent::PlayFireSoundCue()
{
	if (FireSoundCue)
	{
		UGameplayStatics::PlaySound2D(this, FireSoundCue);
	}
}

void UEffectPlayerComponent::PlayBarrelMuzzleFlash()
{
	if (MuzzleFlash && OwnerRef->GetEquippedWeapon())
	{
		FTransform BarrelSocketTransform = OwnerRef->GetEquippedWeapon()->GetBarrelSocketTransform();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BarrelSocketTransform);
	}
}