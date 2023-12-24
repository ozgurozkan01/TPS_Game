// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BaseItem.h"

#include "Character/ShooterCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/InformationPopUp.h"

ABaseItem::ABaseItem() :
	SinusodialSpeed(2.f),
	AmplitudeMultiplier(2.5f),
	YawRotationRate(0.4f),
	ItemName("Default"),
	InformationWidgetObject(nullptr),
	ItemRarity(EItemRarity::EIR_Common),
	ItemState(EItemState::EIS_Pickup),
	bCanIdleMove(true)
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
	
	InformationWidgetComponent = CreateDefaultSubobject<UWidgetComponent>("Information Pop Up");
	InformationWidgetComponent->SetupAttachment(GetRootComponent());
	InformationWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InformationWidgetComponent->SetDrawSize(FVector2D(400.f, 125.f));
	InformationWidgetComponent->SetRelativeLocation(InformationWidgetComponent->GetRelativeLocation() + FVector(0.f, 0.f, 50.f));

	TraceCheckSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Trace Check Area"));
	TraceCheckSphere->SetupAttachment(GetRootComponent());
	TraceCheckSphere->SetSphereRadius(200.f);
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

	for (uint8 i = 0; i < 5; i++)
	{
		ActiveStars.Add(false);
	}

	SetActiveStarts();
	
	InformationWidgetObject = Cast<UInformationPopUp>(InformationWidgetComponent->GetUserWidgetObject());

	if (InformationWidgetObject)
	{
		InformationWidgetObject->SetItemNameText(ItemName);
		InformationWidgetObject->SetStarsImagesVisibility(ActiveStars);
	}

	if (InformationWidgetComponent)
	{
		InformationWidgetComponent->SetVisibility(false);
	}

	TraceCheckSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnSphereBeginOverlap);
	TraceCheckSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnSphereEndOverlap);

	SetItemState(ItemState);
}

void ABaseItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SinusodialMovement();
	Rotate();
}

void ABaseItem::SinusodialMovement()
{
	if (bCanIdleMove == false) { return; }
	
	FVector CurrentLocation { GetActorLocation() };
	CurrentLocation.Z += FMath::Sin(GetGameTimeSinceCreation() * SinusodialSpeed) / AmplitudeMultiplier;

	SetActorLocation(CurrentLocation);	
}

void ABaseItem::Rotate()
{
	if (bCanIdleMove == false) { return; }
	
	FRotator CurrentRotation { GetActorRotation() };
	CurrentRotation.Yaw += YawRotationRate;

	SetActorRotation(CurrentRotation);
}

void ABaseItem::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		TObjectPtr<AShooterCharacter> ShooterCharacter = Cast<AShooterCharacter>(OtherActor);
		
		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(1);
			InformationWidgetObject->SetAmmoAmountText(3);
		}
	}	
}

void ABaseItem::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor)
	{
		TObjectPtr<AShooterCharacter> ShooterCharacter = Cast<AShooterCharacter>(OtherActor);

		if (ShooterCharacter)
		{
			ShooterCharacter->IncrementOverlappedItemCount(-1);
		}
	}
}

void ABaseItem::SetActiveStarts()
{
	uint8 ActiveStarAmount = GetActivateStarNumber();
	
	for (uint8 i = 0; i < ActiveStarAmount; i++)
	{
		ActiveStars[i] = true;
	}
}

void ABaseItem::SetItemCollisions(bool bCanCollide)
{
	ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	if (bCanCollide)
	{
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
		TraceCheckSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		TraceCheckSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	}

	else
	{
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		TraceCheckSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	}
}

void ABaseItem::SetItemProperties(EItemState CurrentState)
{
	switch (CurrentState)
	{
	case EItemState::EIS_Pickup:
		// Set Mesh Properties
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		// Set CollisionBox Properties
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		// Set TraceCheckSphere Properties
		TraceCheckSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
		TraceCheckSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		break;
	
	case EItemState::EIS_Equipped:
		ItemMesh->SetSimulatePhysics(false);
		ItemMesh->SetEnableGravity(false);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		
		TraceCheckSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		TraceCheckSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Falling:
		ItemMesh->SetSimulatePhysics(true);
		ItemMesh->SetEnableGravity(true);
		ItemMesh->SetVisibility(true);
		ItemMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ItemMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		ItemMesh->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);

		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		
		TraceCheckSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		TraceCheckSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		
	}
}

void ABaseItem::SetItemState(EItemState CurrentState)
{
	ItemState = CurrentState;
	SetItemProperties(CurrentState);
}

uint8 ABaseItem::GetActivateStarNumber()
{
	switch (ItemRarity)
	{
	case EItemRarity::EIR_Damaged:
		return 1;
	case EItemRarity::EIR_Common:
		return 2;
	case EItemRarity::EIR_Uncommon:
		return 3;
	case EItemRarity::EIR_Rare:
		return 4;
	case EItemRarity::EIR_Legendary:
		return 5;
	case EItemRarity::EIR_MAX:
		return 0;
	}
	return 0;
}
