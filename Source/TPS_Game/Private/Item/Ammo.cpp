#include "Item/Ammo.h"

#include "Character/ShooterCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/Item/AmmoWidget.h"

AAmmo::AAmmo() :
	AmmoType(EAmmoType::EAT_9mm),
	AmmoCount(30)
	
{
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);

	CollisionBox->SetupAttachment(GetRootComponent());
	InformationWidgetComponent->SetupAttachment(GetRootComponent());
	TraceCheckSphere->SetupAttachment(GetRootComponent());

	AmmoPickUpCollision = CreateDefaultSubobject<USphereComponent>(TEXT("AmmoPickupCollision"));
	AmmoPickUpCollision->SetupAttachment(GetRootComponent());
}

void AAmmo::BeginPlay()
{
	Super::BeginPlay();

	AmmoPickUpCollision->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::AmmoOverlapBegin);
	
	AmmoWidget = Cast<UAmmoWidget>(InformationWidgetComponent->GetUserWidgetObject());
	
	if (AmmoWidget)
	{
		if (AmmoIcon)
		{
			AmmoWidget->SetAmmoIcon(AmmoIcon);
		}
		AmmoWidget->SetAmmoAmount(AmmoCount);
	}

	if (InformationWidgetComponent)
	{
		InformationWidgetComponent->SetVisibility(false);
	}

	ItemType = EItemType::EIT_Ammo;
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

void AAmmo::AmmoOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		TObjectPtr<AShooterCharacter> ShooterChar = Cast<AShooterCharacter>(OtherActor);
		if (ShooterChar)
		{
			StartItemCurve(ShooterChar);
			AmmoPickUpCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		}
	}
}

void AAmmo::SetCustomDepthEnabled(bool bIsEnabled)
{
	Super::SetCustomDepthEnabled(bIsEnabled);
	AmmoMesh->SetRenderCustomDepth(bIsEnabled);
}
