// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/Ammo.h"

#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

AAmmo::AAmmo()
{
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);

	CollisionBox->SetupAttachment(GetRootComponent());
	InformationWidgetComponent->SetupAttachment(GetRootComponent());
	TraceCheckSphere->SetupAttachment(GetRootComponent());
}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();
}

void AAmmo::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void AAmmo::SetItemProperties(EItemState CurrentState)
{
	Super::SetItemProperties(CurrentState);

	switch (CurrentState)
	{
	case EItemState::EIS_Pickup:
		// Set Mesh Properties
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		break;
	
	case EItemState::EIS_Equipped:
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		break;
	case EItemState::EIS_Falling:
		AmmoMesh->SetSimulatePhysics(true);
		AmmoMesh->SetEnableGravity(true);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		AmmoMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		break;
	case EItemState::EIS_EquipInterping:
		AmmoMesh->SetSimulatePhysics(false);
		AmmoMesh->SetEnableGravity(false);
		AmmoMesh->SetVisibility(true);
		AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		AmmoMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		break;
	case EItemState::EIS_PickedUp:
		break;
	case EItemState::EIS_MAX:
		break;
	}
}
