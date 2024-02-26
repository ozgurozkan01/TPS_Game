#include "Components/EffectPlayerComponent.h"
#include "Character/ShooterCharacter.h"
#include "Item/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

UEffectPlayerComponent::UEffectPlayerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UEffectPlayerComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerRef == nullptr)
	{
		OwnerRef = Cast<AShooterCharacter>(GetOwner());
	}
}

/*void UEffectPlayerComponent::PlayHitParticle(const FVector& HitLocation)
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
		/** Spawn the Beam Particle and Store in the variable #1#
		TObjectPtr<UParticleSystemComponent> BeamParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeBeamParticle, Start);
		/** This Particle has the target point and target point represents the end location. If we do not set, the end point is set FVector(0, 0, 0)
		 * To set this variable we need to store it in variable. #1#
		BeamParticleComponent->SetVectorParameter(FName("Target"), End);
	}
}*/

/*

}*/