// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BaseItem.h"

#include "AIHelpers.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "HUD/InformationPopUp.h"
#include "Character/ShooterCharacter.h"
#include "Kismet/GameplayStatics.h"

ABaseItem::ABaseItem() :
	SinusodialSpeed(2.f),
	AmplitudeMultiplier(2.5f),
	YawRotationRate(0.4f),
	ItemName("Default"),
	InformationWidgetObject(nullptr),
	ItemRarity(EItemRarity::EIR_Common),
	ItemState(EItemState::EIS_Pickup),
	bCanIdleMove(true),
	// Interpolation variables
	ZCurveTime(0.7f),
	bIsInterping(false),
	CameraTargetLocation(FVector(0.f)),
	ItemStartInterpLocation(FVector(0.f)),
	ItemInterpingX(0.f),
	ItemInterpingY(0.f),
	ItemInitialYawOffset(0.f)
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

	if (bIsInterping)
	{
		ItemInterp(DeltaTime);
	}
}

void ABaseItem::FinishInterping()
{
	if (ShooterRef)
	{
		ShooterRef->GetPickUpItem(this);
	}

	bIsInterping = false;

	SetActorScale3D(FVector(1.f));
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

void ABaseItem::ItemInterp(float DeltaTime)
{
	if (!bIsInterping || ShooterRef == nullptr || ItemZCurve == nullptr || ItemScaleCurve == nullptr) { return; }


	const float ElapsedTime = GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer);
	const float CurveValue = ItemZCurve->GetFloatValue(ElapsedTime); // Returns the corresponding value to the elapsed time
	FVector ItemLocation = ItemStartInterpLocation; // CurrentLocation

	CameraTargetLocation = ShooterRef->GetCameraInterpLocation();
	// Vector from the ItemLocation to the Camera but at only Z direction. X and Y iz zero
	const FVector ItemToCamera {FVector(0.f, 0.f, (CameraTargetLocation - ItemLocation).Z)};
	// Scale Factor to muliply with CurveValue
	const float SizeZ = ItemToCamera.Size();
	// Adding Curve Value to the Z component of the Inital Location (Scaled by DeltaZ) 
	ItemLocation.Z += CurveValue * SizeZ;

	FVector CurrentLocation { GetActorLocation() };
	/** In this Interpolation, the InterpSpeed might be different from to computer
	 * to computer. U should try to better speed value that suits you on your own */
	const float InterpingXValue = FMath::FInterpTo(CurrentLocation.X, CameraTargetLocation.X, DeltaTime, 30.f);
	const float InterpingYValue = FMath::FInterpTo(CurrentLocation.Y, CameraTargetLocation.Y, DeltaTime, 30.f);

	ItemLocation.X = InterpingXValue;
	ItemLocation.Y = InterpingYValue;
	
	SetActorLocation(ItemLocation, true, nullptr, ETeleportType::TeleportPhysics);

	// Get the camera current rotation to rotate the camera with using offset yaw
	const FRotator CameraRotation { ShooterRef->GetFollowCamera()->GetComponentRotation() };
	// Get the item current rotation and set accordingly camera yaw and initial yaw offset
	FRotator ItemNewRotation {0.f, CameraRotation.Yaw + ItemInitialYawOffset, 0.f};
	// Set the item rotation with new rotation
	SetActorRotation(ItemNewRotation, ETeleportType::TeleportPhysics);

	const float ScaleCurveElapsed { GetWorldTimerManager().GetTimerElapsed(ItemInterpTimer)};
	SetActorScale3D(FVector(ItemScaleCurve->GetFloatValue(ScaleCurveElapsed)));
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

void ABaseItem::StartItemCurve(TObjectPtr<AShooterCharacter> Shooter)
{
	if (Shooter == nullptr) { return; }

	// Set Shooter ref to use after
	ShooterRef = Shooter;
	// Set Interpolation variables
	ItemStartInterpLocation = GetActorLocation();
	bIsInterping = true;
	SetItemState(EItemState::EIS_EquipInterping);

	GetWorldTimerManager().SetTimer(ItemInterpTimer, this, &ABaseItem::FinishInterping, ZCurveTime);

	// Yaw value of camera rotation value
	const double CameraYawRotation { ShooterRef->GetFollowCamera()->GetComponentRotation().Yaw };
	// Yaw value of item rotation value
	const double ItemYawRotation {GetActorRotation().Yaw};
	// Yaw rotation offset between item and camera (angle difference)
	ItemInitialYawOffset = ItemYawRotation - CameraYawRotation;
}

void ABaseItem::PlayPickupSoundCue()
{
	if (PickupSoundCue)
	{
		UGameplayStatics::PlaySound2D(this, PickupSoundCue);
	}
}

void ABaseItem::PlayEquipSoundCue()
{
	if (EquipSoundCue)
	{
		UGameplayStatics::PlaySound2D(this, EquipSoundCue);
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
		break;
	case EItemState::EIS_EquipInterping:
		InformationWidgetComponent->SetVisibility(false);
		
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
