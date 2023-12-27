// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon.h"

#include "Engine/SkeletalMeshSocket.h"
#include "HUD/InformationPopUp.h"


AWeapon::AWeapon() :
	MaxAmmoAmount(30),
	CurrentAmmoAmount(MaxAmmoAmount),
	ThrowWeaponTime(0.85),
	bIsFalling(false)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	if (GetInformationWidgetObject())
	{
		GetInformationWidgetObject()->SetAmmoAmountText(CurrentAmmoAmount);
	}
}

void AWeapon::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetItemState() == EItemState::EIS_Falling && bIsFalling)
	{
		const FRotator MeshRotation {0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f};
		GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);
	}
	
}

void AWeapon::ThrowWeapon()
{
	FRotator MeshRotation { 0.f, GetItemMesh()->GetComponentRotation().Yaw, 0.f};
	GetItemMesh()->SetWorldRotation(MeshRotation, false, nullptr, ETeleportType::TeleportPhysics);

	const FVector MeshForwardVector { GetItemMesh()->GetForwardVector() };
	const FVector MeshRightVector { GetItemMesh()->GetRightVector() };
	
	/** MeshRightVector rotates 20 degrees downwards around forward vector axis */
	FVector ImpulseDirection = MeshRightVector.RotateAngleAxis(-20, MeshForwardVector);

	float RandomDegreeAroundZ {FMath::RandRange(-30.f, 30.f)};

	/** ImpulseDirection rotates -30-30 degrees around up vector axis */
	ImpulseDirection = ImpulseDirection.RotateAngleAxis(RandomDegreeAroundZ, FVector::UpVector);

	float ForceAmount { FMath::RandRange(7.f, 15.f)};
	ForceAmount *= 1'000.f;
	GetItemMesh()->AddImpulse(ImpulseDirection * ForceAmount);

	bIsFalling = true;

	GetWorldTimerManager().SetTimer(ThrowWeaponTimer, this, &AWeapon::StopFalling, ThrowWeaponTime);
}

void AWeapon::StopFalling()
{
	bIsFalling = false;
	SetItemState(EItemState::EIS_Pickup);
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