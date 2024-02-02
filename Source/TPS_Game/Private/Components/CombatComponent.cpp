#include "Components/CombatComponent.h"
#include "Character/ShooterCharacter.h"
#include "Components/AnimatorComponent.h"
#include "Components/EffectPlayerComponent.h"
#include "Components/TracerComponent.h"
#include "Item/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Components/MotionComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/CharacterMovementComponent.h"

UCombatComponent::UCombatComponent() :
	// Automatic Gun Fire Factors
	AutomaticFireRate(0.1f),
	CombatState(ECombatState::ECS_Unoccupied),
	bAiming(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerRef == nullptr)
	{
		OwnerRef = Cast<AShooterCharacter>(GetOwner());
	}
}
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UCombatComponent::ShootingStart()
{
	FVector BeamEndPoint = OwnerRef->GetTracerComponent()->GetBeamEndPoint();
	FTransform BarrelSocketTransform = OwnerRef->GetEquippedWeapon()->GetBarrelSocketTransform();
	OwnerRef->GetAnimatorComponent()->PlayGunFireMontage();
	OwnerRef->GetEffectPlayerComponent()->PlayHitParticle(BeamEndPoint);
	OwnerRef->GetEffectPlayerComponent()->PlayBeamParticle(BarrelSocketTransform, BeamEndPoint);
	OwnerRef->GetEffectPlayerComponent()->PlayFireSoundCue();
	OwnerRef->GetEffectPlayerComponent()->PlayBarrelMuzzleFlash();
	OwnerRef->GetEquippedWeapon()->DecremenetAmmo();
	StartFireTimer();
}

void UCombatComponent::StartAim()
{
	SetIsScopeOpen(true);
	SetIsAiming(true);
	OwnerRef->GetCharacterMovement()->MaxWalkSpeed = OwnerRef->GetMotionComponent()->GetCrouchingSpeed();
}

void UCombatComponent::StopAim()
{
	SetIsAiming(false);
	SetIsScopeOpen(false);
	OwnerRef->GetCharacterMovement()->MaxWalkSpeed = OwnerRef->GetMotionComponent()->GetRunningSpeed();
}

void UCombatComponent::StartFireTimer()
{
	CombatState = ECombatState::ECS_FireTimerInProgress;
	OwnerRef->GetWorldTimerManager().SetTimer(AutomaticFireHandle, this, &UCombatComponent::AutomaticFireReset, AutomaticFireRate);
}

void UCombatComponent::AutomaticFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;
	if (OwnerRef && OwnerRef->GetEquippedWeapon())
	{
		if (OwnerRef->GetEquippedWeapon()->GetCurrentAmmo() > 0)
		{
			if (OwnerRef->GetIsFireButtonPressed())
			{
				StartFireTimer();
			}	
		}

		else
		{
			ReloadWeapon();
		}	
	}
}

void UCombatComponent::ReloadWeapon()
{
	if (CombatState == ECombatState::ECS_Unoccupied && OwnerRef->CarryingAmmo() && !OwnerRef->GetEquippedWeapon()->IsMagazineFull())
	{
		if (GetIsAiming())
		{
			StopAim();
		}
		CombatState = ECombatState::ECS_Reloading;
		OwnerRef->GetAnimatorComponent()->PlayReloadWeaponMontage();
	}
}


void UCombatComponent::FinishReloading()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (bIsScopeOpen)
	{
		StartAim();
	}

	if (OwnerRef->GetEquippedWeapon())
	{
		const EAmmoType AmmoType = OwnerRef->GetEquippedWeapon()->GetAmmoType();

		if (OwnerRef->GetAmmoMap().Contains(AmmoType))
		{
			// Returns to us useable ammo amount of that AmmoType
			int32 CarriedAmmo = OwnerRef->GetAmmoMap()[AmmoType];
			// Space left in the magazine of EquippedWeapon
			const int32 MagazineEmptySpace = OwnerRef->GetEquippedWeapon()->GetMagazineCapacity() - OwnerRef->GetEquippedWeapon()->GetCurrentAmmo();

			if (MagazineEmptySpace > CarriedAmmo)
			{
				// Reload the weapon with carried ammo
				OwnerRef->GetEquippedWeapon()->ReloadAmmo(CarriedAmmo);
				CarriedAmmo = 0;
				OwnerRef->UpdateAmmoMap(AmmoType, CarriedAmmo);
			}

			else
			{
				// Fill the weapon
				OwnerRef->GetEquippedWeapon()->ReloadAmmo(MagazineEmptySpace);
				CarriedAmmo -= MagazineEmptySpace;
				OwnerRef->UpdateAmmoMap(AmmoType, CarriedAmmo);
			}
		}	
	}
}