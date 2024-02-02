// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AnimatorComponent.h"
#include "AnimInstance/ShooterAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "Item/Weapon.h"

UAnimatorComponent::UAnimatorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAnimatorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (ShooterAnimInstance == nullptr && OwnerRef)
	{
		TObjectPtr<UAnimInstance> AnimInstance = OwnerRef->GetMesh()->GetAnimInstance();
		ShooterAnimInstance = Cast<UShooterAnimInstance>(AnimInstance);
	}
}

void UAnimatorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UAnimatorComponent::PlayGunFireMontage()
{
	if (ShooterAnimInstance && GunFireMontage)
	{
		ShooterAnimInstance->Montage_Play(GunFireMontage);
		ShooterAnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void UAnimatorComponent::PlayReloadWeaponMontage()
{
	if (ShooterAnimInstance && ReloadMontage && OwnerRef->GetEquippedWeapon())
	{
		ShooterAnimInstance->Montage_Play(ReloadMontage);
		ShooterAnimInstance->Montage_JumpToSection(OwnerRef->GetEquippedWeapon()->GetReloadingWeaponSection());
	}
}
