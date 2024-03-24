// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WeaponAnimInstance.h"
#include "Character/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Item/Weapon.h"

void UWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (ShooterRef == nullptr)
	{
		ShooterRef = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	}

	if (OwningWeaponRef == nullptr)
	{
		OwningWeaponRef = Cast<AWeapon>(GetOwningActor());
	}
}

void UWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (ShooterRef == nullptr)
	{
		ShooterRef = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	}

	if (OwningWeaponRef == nullptr)
	{
		OwningWeaponRef = Cast<AWeapon>(GetOwningActor());
	}
	
	if (ShooterRef && OwningWeaponRef)
	{
		LeftHandSceneCompTransform = ShooterRef->GetLeftHandSceneCompTransform();
		bIsMovingMagazine = OwningWeaponRef->IsMovingClip();

		if (OwningWeaponRef->GetWeaponType() == EWeaponType::EWT_Pistol)
		{
			SlideDisplacement = OwningWeaponRef->GetSlideDisplacement() * -1.f;
			RecoilRotation = OwningWeaponRef->GetRecoilRotation() * -1.f;
		}
	}
}