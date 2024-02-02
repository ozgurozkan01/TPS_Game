// Fill out your copyright notice in the Description page of Project Settings.


#include "Item/BaseItem.h"

#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/ShooterCharacter.h"
#include "Curves/CurveVector.h"
#include "Kismet/GameplayStatics.h"

ABaseItem::ABaseItem() :
	SinusodialSpeed(2.f),
	AmplitudeMultiplier(2.5f),
	YawRotationRate(0.4f),
	ItemName("Default"),
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
	ItemInitialYawOffset(0.f),
	InterpLocationIndex(0),
	ItemType(EItemType::EIT_Default),
	// Dynamic Material Parameters
	GlowAmount(150.f),
	FresnelExponent(3.f),
	FresnelReflectFraction(4.f),
	PulseCurveTime(5.f)
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

void ABaseItem::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	if (MaterialInstance)
	{
		DynamicMaterialInstance = UMaterialInstanceDynamic::Create(MaterialInstance, this);
		ItemMesh->SetMaterial(0, DynamicMaterialInstance);
	}

	SetGlowMaterialEnabled(1.f);
}

void ABaseItem::BeginPlay()
{
	Super::BeginPlay();

	TraceCheckSphere->OnComponentBeginOverlap.AddDynamic(this, &ABaseItem::OnSphereBeginOverlap);
	TraceCheckSphere->OnComponentEndOverlap.AddDynamic(this, &ABaseItem::OnSphereEndOverlap);

	SetItemState(ItemState);
	SetCustomDepthEnabled(false);
	SetGlowMaterialEnabled(0.1f);

	StartGlowPulseTimer();
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

	UpdateGlowPulseScalars();
}

void ABaseItem::FinishInterping()
{
	if (ShooterRef)
	{
		ShooterRef->GetPickUpItem(this);
		// Update the location interping item count 
		ShooterRef->UpdateInterpingItemCount(InterpLocationIndex, -1);
	}

	bIsInterping = false;

	SetActorScale3D(FVector(1.f));
	SetGlowMaterialEnabled(1.f);
	SetCustomDepthEnabled(false);
	PlayEquipSoundCue();
}

void ABaseItem::StartGlowPulseTimer()
{
	if(ItemState == EItemState::EIS_Pickup)
	{
		GetWorldTimerManager().SetTimer(PulseTimerHandle, this, &ABaseItem::ResetGlowPulseTimer, PulseCurveTime);
	}
}

void ABaseItem::ResetGlowPulseTimer()
{
	StartGlowPulseTimer();
}

void ABaseItem::UpdateGlowPulseScalars()
{
	if (ItemState == EItemState::EIS_Pickup && GlowPulseCurve)
	{
		const float ElapsedTime { GetWorldTimerManager().GetTimerElapsed(PulseTimerHandle)};
		const FVector PulseCurve { GlowPulseCurve->GetVectorValue(ElapsedTime)}; 
		
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowAmount"), GlowAmount * PulseCurve.X);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelExponent"), FresnelExponent * PulseCurve.Y);
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("FresnelReflectFraction"), FresnelReflectFraction * PulseCurve.Z);
	}
}

void ABaseItem::SetCustomDepthEnabled(bool bIsEnabled)
{
	if (ItemMesh)
	{
		ItemMesh->SetRenderCustomDepth(bIsEnabled);
	}
}

void ABaseItem::SetGlowMaterialEnabled(float Value)
{
	if (DynamicMaterialInstance)
	{
		DynamicMaterialInstance->SetScalarParameterValue(TEXT("GlowBlendAlpha"), Value);
	}
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

	CameraTargetLocation = GetInterpLocCorrespondItemType();
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

FVector ABaseItem::GetInterpLocCorrespondItemType()
{
	switch (ItemType)
	{
	case EItemType::EIT_Ammo:
		return ShooterRef->GetInterpLocation(InterpLocationIndex).InterpLocation->GetComponentLocation();
	case EItemType::EIT_Weapon:
		return ShooterRef->GetInterpLocation(0).InterpLocation->GetComponentLocation();
	}
	
	return FVector::ZeroVector;
}

void ABaseItem::StartItemCurve(TObjectPtr<AShooterCharacter> Shooter)
{
	if (Shooter)
	{
		PlayPickupSoundCue();
		// Set Shooter ref to use after
		ShooterRef = Shooter;
		// Get correct interping location index to start curve behaviour
		InterpLocationIndex = ShooterRef->GetInterpLocationIndex();
		// Update the location interping item count 
		ShooterRef->UpdateInterpingItemCount(InterpLocationIndex, 1);

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
		SetCustomDepthEnabled(false);
		SetGlowMaterialEnabled(1.f);
	}
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
		// Set CollisionBox Properties
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		CollisionBox->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		// Set TraceCheckSphere Properties
		TraceCheckSphere->SetCollisionResponseToAllChannels(ECR_Overlap);
		TraceCheckSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		break;
	
	case EItemState::EIS_Equipped:
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		
		TraceCheckSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		TraceCheckSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_Falling:
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		
		TraceCheckSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		TraceCheckSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_EquipInterping:
		CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		CollisionBox->SetCollisionResponseToAllChannels(ECR_Ignore);
		
		TraceCheckSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
		TraceCheckSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EItemState::EIS_PickedUp:
		break;
	case EItemState::EIS_MAX:
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
