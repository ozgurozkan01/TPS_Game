// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "HUD/InformationPopUp.h"


AWeapon::AWeapon() :
	MaxAmmoAmount(30),
	CurrentAmmoAmount(MaxAmmoAmount)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
	GetInformationWidgetObject()->SetAmmoAmountText(CurrentAmmoAmount);
}

const USkeletalMeshSocket* AWeapon::GetBarrelSocket() const
{
	if (GetItemSkeletalMesh())
	{
		return GetItemSkeletalMesh()->GetSocketByName("BarrelSocket");
	}

	return nullptr;
}

FTransform AWeapon::GetBarrelSocketTransform() const
{
	if (GetBarrelSocket())
	{
		return GetBarrelSocket()->GetSocketTransform(GetItemSkeletalMesh());
	}

	return FTransform();
}

void AWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}
