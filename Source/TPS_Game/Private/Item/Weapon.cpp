// Fill out your copyright notice in the Description page of Project Settings.

#include "Item/Weapon.h"
#include "Character/ShooterCharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "HUD/Item/InformationPopUp.h"
#include "Item/Ammo.h"
#include "Item/WeaponRarityTable.h"
#include "Item/WeaponTable.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AWeapon::AWeapon() :
	MagazineCapacity(30),
	InformationWidgetObject(nullptr),
	CurrentAmmoAmount(MagazineCapacity),
	ThrowWeaponTime(0.85),
	bIsFalling(false),
	WeaponType(EWeaponType::EWT_SubmachineGun),
	AmmoType(EAmmoType::EAT_9mm),
	ReloadingMontageSection(FName(TEXT("Reload SMG"))),
	bIsMovingMagazine(false),
	SlotIndex(0)
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponRarityTableRef(TEXT("/Script/Engine.DataTable'/Game/_Game/BP/DataTable/DT_WeaponRarity.DT_WeaponRarity'"));
	if (WeaponRarityTableRef.Succeeded())
	{
		WeaponRarityDataTable = WeaponRarityTableRef.Object;
	}

	static ConstructorHelpers::FObjectFinder<UDataTable> WeaponTableRef(TEXT("/Script/Engine.DataTable'/Game/_Game/BP/DataTable/DT_Weapon.DT_Weapon'"));
	if (WeaponTableRef.Succeeded())
	{
		WeaponDataTable = WeaponTableRef.Object;
	}
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	SetWeaponRarityTableProperties();
	SetWeaponTableProperties();
	
	InformationWidgetObject = Cast<UInformationPopUp>(InformationWidgetComponent->GetUserWidgetObject());

	if (InformationWidgetObject)
	{
		InformationWidgetObject->SetAmmoAmountText(CurrentAmmoAmount);
		InformationWidgetObject->SetItemNameText(ItemName);
		InformationWidgetObject->SetStarsImagesVisibility(ActiveStar);
		InformationWidgetObject->SetRightBoxBackgroundColors(BrightColor, DarkColor);
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

void AWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Super::OnSphereEndOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex);

	TObjectPtr<AShooterCharacter> ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
	
	if (ItemState == EItemState::EIS_Pickup && ShooterCharacter && ShooterCharacter->GetInventoryComponent() && ShooterCharacter->GetInventoryComponent()->GetHighlightSlotIndex() != -1)
	{
		ShooterCharacter->GetInventoryComponent()->SetInventorySlotHightlight(false);
	}
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

void AWeapon::SetWeaponRarityTableProperties()
{
	if (WeaponRarityDataTable)
	{
		FWeaponRarityTable* WeaponRarityRow = nullptr;

		switch (ItemRarity)
		{
		case EItemRarity::EIR_Damaged:
			WeaponRarityRow = WeaponRarityDataTable->FindRow<FWeaponRarityTable>(FName("Damaged"), TEXT(""));
			break;
		case EItemRarity::EIR_Common:
			WeaponRarityRow = WeaponRarityDataTable->FindRow<FWeaponRarityTable>(FName("Common"), TEXT(""));
			break;
		case EItemRarity::EIR_Uncommon:
			WeaponRarityRow = WeaponRarityDataTable->FindRow<FWeaponRarityTable>(FName("Uncommon"), TEXT(""));
			break;
		case EItemRarity::EIR_Rare:
			WeaponRarityRow = WeaponRarityDataTable->FindRow<FWeaponRarityTable>(FName("Rare"), TEXT(""));
			break;
		case EItemRarity::EIR_Legendary:
			WeaponRarityRow = WeaponRarityDataTable->FindRow<FWeaponRarityTable>(FName("Legendary"), TEXT(""));
		}

		if (WeaponRarityRow)
		{
			GlowColor = WeaponRarityRow->GlowColor;
			BrightColor = WeaponRarityRow->BrightColor;
			DarkColor = WeaponRarityRow->DarkColor;
			ActiveStar = WeaponRarityRow->ActiveStar;
			SlotBackgroundImage = WeaponRarityRow->SlotBackgroundImage;

			if (GetItemMesh())
			{
				if (DynamicMaterialInstance)
				{
					DynamicMaterialInstance->SetVectorParameterValue(TEXT("FresnelColor"), GlowColor);
					ItemMesh->SetMaterial(0, DynamicMaterialInstance);
				}
				
				GetItemMesh()->SetCustomDepthStencilValue(WeaponRarityRow->CustomDepthStencil);
			}
		}
	}
}

void AWeapon::SetWeaponTableProperties()
{
	if (WeaponDataTable)
	{
		FWeaponTable* WeaponTableRow = nullptr;
		
		switch (WeaponType)
		{
		case EWeaponType::EWT_SubmachineGun:
			WeaponTableRow = WeaponDataTable->FindRow<FWeaponTable>(FName("SubmachineGun"), TEXT(""));
			break;
		case EWeaponType::EWT_AssaultRifle:
			WeaponTableRow = WeaponDataTable->FindRow<FWeaponTable>(FName("AssaultRifle"), TEXT(""));
			break;
		}

		if (WeaponTableRow)
		{
			AmmoType = WeaponTableRow->AmmoType;
			CurrentAmmoAmount = WeaponTableRow->CurrentAmmoAmount;
			MagazineCapacity = WeaponTableRow->MazagineCapacity;
			PickupSound = WeaponTableRow->PickupSound;
			EquipSound = WeaponTableRow->EquipSound;
			ItemMesh->SetSkeletalMesh(WeaponTableRow->WeaponMesh);
			AmmoIcon = WeaponTableRow->AmmoIcon;
			WeaponIcon = WeaponTableRow->WeaponIcon;
			ItemName = WeaponTableRow->WeaponName;
			MaterialInstance = WeaponTableRow->MaterialInstance;
			MaterialIndex = WeaponTableRow->MaterialIndex;
			MagazineBoneName = WeaponTableRow->MagazineBoneName;
			ReloadingMontageSection = WeaponTableRow->ReloadMontageSection;
			ItemMesh->SetAnimInstanceClass(WeaponTableRow->AnimInstanceBP);
			CrosshairLeft = WeaponTableRow->CrosshairLeft;
			CrosshairRight = WeaponTableRow->CrosshairRight;
			CrosshairBottom = WeaponTableRow->CrosshairBottom;
			CrosshairTop = WeaponTableRow->CrosshairTop;
			CrosshairMiddle = WeaponTableRow->CrosshairMiddle;
			FireSoundCue = WeaponTableRow->FireSoundCue;
			AutoFireRate = WeaponTableRow->AutoFireRate;
			
			if (MaterialInstance)
			{
				DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
				DynamicMaterialInstance->SetVectorParameterValue(TEXT("FresnelColor"), GlowColor);
				ItemMesh->SetMaterial(MaterialIndex, DynamicMaterialInstance);
			}
				
		}
	}
}

void AWeapon::PlayFireSoundCue()
{
	if (FireSoundCue)
	{
		UGameplayStatics::PlaySound2D(this, FireSoundCue);
	}
}

void AWeapon::PlayBarrelMuzzleFlash()
{
	if (MuzzleFlash)
	{
		FTransform BarrelSocketTransform = GetBarrelSocketTransform();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BarrelSocketTransform);
	}
}

void AWeapon::PlayHitParticle(const FVector& HitLocation)
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitLocation);
	}
}

void AWeapon::PlayBeamParticle(const FTransform& Start, const FVector& End)
{
	if (SmokeBeamParticle)
	{
		/** Spawn the Beam Particle and Store in the variable */
		TObjectPtr<UParticleSystemComponent> BeamParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeBeamParticle, Start);
		/** This Particle has the target point and target point represents the end location. If we do not set, the end point is set FVector(0, 0, 0)
		 * To set this variable we need to store it in variable. */
		BeamParticleComponent->SetVectorParameter(FName("Target"), End);
	}
}