// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/WeaponAnimInstance.h"

#include "Character/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"

void UWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (ShooterRef == nullptr)
	{
		ShooterRef = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	}
}

void UWeaponAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	if (ShooterRef == nullptr)
	{
		ShooterRef = Cast<AShooterCharacter>(UGameplayStatics::GetPlayerPawn(this, 0));
	}

	if (ShooterRef)
	{
		LeftHandSceneCompTransform = ShooterRef->GetLeftHandSceneComp()->GetComponentTransform();
	}
}