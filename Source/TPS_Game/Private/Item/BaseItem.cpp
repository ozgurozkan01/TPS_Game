// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BaseItem.h"
#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"

ABaseItem::ABaseItem() :
	SinusodialSpeed(2.f),
	AmplitudeMultiplier(2.5f),
	YawRotationRate(0.4f)
{
	PrimaryActorTick.bCanEverTick = true;

	ItemMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Item Mesh"));
	SetRootComponent(ItemMesh);

	CollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision Box"));
	CollisionBox->SetupAttachment(GetRootComponent());
	CollisionBox->SetGenerateOverlapEvents(true);
	CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionBox->SetCollisionObjectType(ECC_WorldStatic);
	CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
	CollisionBox->SetCollisionResponseToChannel(ECC_Camera, ECR_Overlap);
	
	InformationPopUpWidget = CreateDefaultSubobject<UWidgetComponent>("Information Pop Up");
	InformationPopUpWidget->SetupAttachment(GetRootComponent());
	InformationPopUpWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InformationPopUpWidget->SetDrawSize(FVector2D(400.f, 125.f));
	InformationPopUpWidget->SetRelativeLocation(InformationPopUpWidget->GetRelativeLocation() + FVector(0.f, 0.f, 50.f));
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

	InformationPopUpWidget->SetVisibility(false);
}

void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SinusodialMovement();
	Rotate();
}

void ABaseItem::SinusodialMovement()
{
	FVector CurrentLocation { GetActorLocation() };
	CurrentLocation.Z += FMath::Sin(GetGameTimeSinceCreation() * SinusodialSpeed) / AmplitudeMultiplier;

	SetActorLocation(CurrentLocation);	
}

void ABaseItem::Rotate()
{
	FRotator CurrentRotation { GetActorRotation() };
	CurrentRotation.Yaw += YawRotationRate;

	SetActorRotation(CurrentRotation);
}