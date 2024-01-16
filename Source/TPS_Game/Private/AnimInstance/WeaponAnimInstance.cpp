// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WeaponAnimInstance.h"

#include "Character/ShooterCharacter.h"
#include "Item/Weapon.h"
#include "Kismet/GameplayStatics.h"

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
		/** Update Hand Scene Component Transform every frame */
		LeftHandSceneCompTransform = ShooterRef->GetLeftHandSceneComp()->GetComponentTransform();
		/** Update moving bool controller every frame */
		bIsMovingMagazine = OwningWeaponRef->IsMovingClip();
	}
}