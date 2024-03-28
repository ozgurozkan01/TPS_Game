// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/AnimatorComponent.h"
#include "AnimInstance/ShooterAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "Components/CombatComponent.h"
#include "Item/Weapon.h"

UAnimatorComponent::UAnimatorComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UAnimatorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (OwnerRef == nullptr)
	{
		OwnerRef = Cast<AShooterCharacter>(GetOwner());
	}
	
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

void UAnimatorComponent::PlayEquipWeaponMontage()
{
	if (ShooterAnimInstance && EquipWeaponMontage)
	{
		ShooterAnimInstance->Montage_Play(EquipWeaponMontage);
		ShooterAnimInstance->Montage_JumpToSection(FName("Equip"));
	}
}

void UAnimatorComponent::FinishEquipping()
{
	if (OwnerRef->GetCombatComponent())
	{
		OwnerRef->GetCombatComponent()->SetCombatState(ECombatState::ECS_Unoccupied);

		if (OwnerRef->GetIsAimingButtonPressed())
		{
			OwnerRef->GetCombatComponent()->SetIsAiming(true);
		}
	}
}

void UAnimatorComponent::GrabMagazine()
{
	if (OwnerRef->GetEquippedWeapon() == nullptr) { return; }
	if (OwnerRef->GetEquippedWeapon()->GetItemMesh() == nullptr) {return;}
	if (OwnerRef->GetLeftHandSceneComponent() == nullptr) { return; }
	
	FName MagazineName = OwnerRef->GetEquippedWeapon()->GetMagazineBoneName();
	int32 MagazineIndex = OwnerRef->GetEquippedWeapon()->GetItemMesh()->GetBoneIndex(MagazineName);
	MagazineTransform = OwnerRef->GetEquippedWeapon()->GetItemMesh()->GetBoneTransform(MagazineIndex);
	/** KeepRelative does the attached component hold its position by getting parent object as a center point. */
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	// SceneComponent attached to the character mesh socket via the socket.
	OwnerRef->GetLeftHandSceneComponent()->AttachToComponent(OwnerRef->GetMesh(), AttachmentTransformRules, FName(TEXT("hand_l")));
	/** when leftHandSceneComponent is attached to the hand_l bone, leftHandSceneComponent center is the bone center.
	 * But when character holds the weapon magazine, magazine location and rotation is updated by using of leftHandSceneComp transform.
	 * In this process, leftHandSceneComponent is the center of hand_l bone and because of that, magazine looks like overlapping with
	 * mesh. To fix this, at the time of grabbing magazine, we update component transform as magazine transform. Because when character close its hand
	 * and grab magazine, magazine looks like the center of the closed hand. */
	OwnerRef->GetLeftHandSceneComponent()->SetWorldTransform(MagazineTransform);
	OwnerRef->GetEquippedWeapon()->SetbIsMovingMagazine(true);
}

void UAnimatorComponent::ReplaceMagazine()
{
	if (OwnerRef->GetEquippedWeapon())
	{
		OwnerRef->GetEquippedWeapon()->SetbIsMovingMagazine(false);
	}
}

void UAnimatorComponent::FinishReloading()
{
	if (OwnerRef->GetCombatComponent())
	{
		OwnerRef->GetCombatComponent()->FinishReloading();
	}
}