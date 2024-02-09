// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon.h"

#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "HUD/Item/InformationPopUp.h"
#include "Item/Ammo.h"

AWeapon::AWeapon() :
	MagazineCapacity(30),
	InformationWidgetObject(nullptr),
	CurrentAmmoAmount(MagazineCapacity),
	ThrowWeaponTime(0.85),
	bIsFalling(false),
	WeaponType(EWeaponType::EWT_SubmachineGun),
	AmmoType(EAmmoType::EAT_9mm),
	ReloadingMontageSection(FName(TEXT("Reload SMG"))),
	MagazineBoneName("smg_clip"),
	bIsMovingMagazine(false),
	SlotIndex(0)
{
	PrimaryActorTick.bCanEverTick = true;
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	for (uint8 i = 0; i < 5; i++)
	{
		ActiveStars.Add(false);
	}

	SetActiveStars();
	
	InformationWidgetObject = Cast<UInformationPopUp>(InformationWidgetComponent->GetUserWidgetObject());

	if (InformationWidgetObject)
	{
		InformationWidgetObject->SetAmmoAmountText(CurrentAmmoAmount);
		InformationWidgetObject->SetItemNameText(ItemName);
		InformationWidgetObject->SetStarsImagesVisibility(ActiveStars);
	}

	if (InformationWidgetComponent)
	{
		InformationWidgetComponent->SetVisibility(false);
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
	StartGlowPulseTimer();
}

void AWeapon::SetItemProperties(EItemState CurrentState)
{
	Super::SetItemProperties(CurrentState);
		switch (CurrentState)
	{
	case EItemState::EIS_Pickup:
		// Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		break;
	
	case EItemState::EIS_Equipped:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		break;
	case EItemState::EIS_Falling:
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		break;
	case EItemState::EIS_EquipInterping:
		InformationWidgetComponent->SetVisibility(false);
		
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		break;
	case EItemState::EIS_PickedUp:
		InformationWidgetComponent->SetVisibility(false);
		
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(false);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		break;
	case EItemState::EIS_MAX:
		break;
	}
}

void AWeapon::DecremenetAmmo()
{
	CurrentAmmoAmount--;

	if (CurrentAmmoAmount <= 0)
	{
		CurrentAmmoAmount = 0;
	}
}

bool AWeapon::IsMagazineFull()
{
	return CurrentAmmoAmount >= MagazineCapacity;
}

void AWeapon::ReloadAmmo(int32 Ammo)
{
	CurrentAmmoAmount += Ammo;
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

void AWeapon::SetActiveStars()
{
	uint8 ActiveStarAmount = GetActivateStarNumber();
	
	for (uint8 i = 0; i < ActiveStarAmount; i++)
	{
		ActiveStars[i] = true;
	}
}