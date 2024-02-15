// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/TracerComponent.h"

#include "Character/ShooterCharacter.h"
#include "Components/InventoryComponent.h"
#include "Components/WidgetComponent.h"
#include "Item/BaseItem.h"
#include "Item/Weapon.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UTracerComponent::UTracerComponent() :
	FireBulletDuration(0.05f),
	bFiring(false),
	bShouldTraceForItems(false),
	BeamEnd(FVector::ZeroVector),
	OverlappedItemCount(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UTracerComponent::BeginPlay()
{
	Super::BeginPlay();
	if (MainCharacter == nullptr)
	{
		MainCharacter = Cast<AShooterCharacter>(GetOwner());
	}
}


// Called every frame
void UTracerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	LineTraceForInformationPopUp();
}


bool UTracerComponent::IsConvertedScreenToWorld(FVector& CrosshairWorldPosition, FVector& CrosshairWorldDirection)
{
	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairPosition = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
	
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairPosition,
		CrosshairWorldPosition,
		CrosshairWorldDirection
		);

	return bScreenToWorld;
}

void UTracerComponent::LineTraceFromTheScreen(const FVector& CrosshairWorldPosition, const FVector& CrosshairWorldDirection, FVector& BeamEndPoint)
{
	FVector Start {CrosshairWorldPosition};
	FVector End {Start + CrosshairWorldDirection * 50'000.f};
	// Set beam end point to line trace end point
	BeamEndPoint = End;
	
	FHitResult ScreenTraceHit;
	// Tracing outward from the crosshairs world location
	GetWorld()->LineTraceSingleByChannel(ScreenTraceHit, Start, End, ECC_Visibility);

	if (ScreenTraceHit.bBlockingHit)
	{
		// Beam end point is set as tracing hit location
		BeamEndPoint = ScreenTraceHit.Location;
	}
}

void UTracerComponent::LineTraceFromTheGunBarrel(const FVector& GunSocketLocation, FVector& BeamEndPoint)
{
	const FVector WeaponTraceStart { GunSocketLocation };
	const FVector WeaponTraceEnd { BeamEndPoint };

	FHitResult WeaponTraceHit;
	// Tracing from the gun barrel to the Screen Trace Hit Location
	// We trace from the camera to the world screen, but we need to trace also
	// from the gun barrel to the screen trace hit location. Because, the gun is more
	// angular than the camera, because of that, where the camera trace hit , gun trace might not arrive.
	GetWorld()->LineTraceSingleByChannel(WeaponTraceHit, WeaponTraceStart, WeaponTraceEnd, ECC_Visibility);
	if (WeaponTraceHit.bBlockingHit)
	{
		BeamEndPoint = WeaponTraceHit.Location;
	}
}

void UTracerComponent::LineTraceForInformationPopUp()
{
	if (!bShouldTraceForItems)
	{
		if (HeldItem)
		{
			HeldItem->GetInformationWidgetComponent()->SetVisibility(false);
			HeldItem->SetCustomDepthEnabled(false);
		}
		return;
	}
	
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = IsConvertedScreenToWorld(CrosshairWorldPosition, CrosshairWorldDirection);
	
	if (bScreenToWorld)
	{
		FVector StartPoint { CrosshairWorldPosition };
		FVector EndPoint { CrosshairWorldPosition + CrosshairWorldDirection * 750.f };
		FHitResult PopUpHit;

		GetWorld()->LineTraceSingleByChannel(PopUpHit, StartPoint, EndPoint, ECC_Visibility);

		if (PopUpHit.bBlockingHit)
		{
			TraceHitItem = Cast<ABaseItem>(PopUpHit.GetActor());
			const TObjectPtr<AWeapon> TraceHitWeapon = Cast<AWeapon>(TraceHitItem);
			
			if (TraceHitWeapon)
			{
				if (MainCharacter && MainCharacter->GetInventoryComponent() && MainCharacter->GetInventoryComponent()->GetHighlightSlotIndex() == -1)
				{
					MainCharacter->GetInventoryComponent()->SetInventorySlotHightlight(true);
				}
			}

			else 
			{
				if (MainCharacter && MainCharacter->GetInventoryComponent() && MainCharacter->GetInventoryComponent()->GetHighlightSlotIndex() != -1)
				{
					MainCharacter->GetInventoryComponent()->SetInventorySlotHightlight(false);
				}
			}
			
			if (TraceHitItem)
			{
				if (TraceHitItem->GetItemState() == EItemState::EIS_EquipInterping)
				{
					TraceHitItem == nullptr;
				}

				if (TraceHitItem->GetInformationWidgetComponent())
				{
					TraceHitItem->GetInformationWidgetComponent()->SetVisibility(true);
					TraceHitItem->SetCustomDepthEnabled(true);
				}
			}

			if (HeldItem && HeldItem != TraceHitItem)
			{
				HeldItem->GetInformationWidgetComponent()->SetVisibility(false);
				HeldItem->SetCustomDepthEnabled(false);
			}
			
			HeldItem = TraceHitItem;
		}

		else
		{
			if (HeldItem)
			{
				HeldItem->GetInformationWidgetComponent()->SetVisibility(false);
				HeldItem->SetCustomDepthEnabled(false);
			}
		}
	}
	
	else if (HeldItem)
	{
		HeldItem->GetInformationWidgetComponent()->SetVisibility(false);
		HeldItem->SetCustomDepthEnabled(false);
	}
}

void UTracerComponent::CrosshairStartFireBullet()
{
	bFiring = true;

	if (GetOwner() && GetWorld())
	{
		GetOwner()->GetWorld()->GetTimerManager().SetTimer(FireBulletHandle, this, &UTracerComponent::CrosshairFinishFireBullet, FireBulletDuration);
	}
}

void UTracerComponent::CrosshairFinishFireBullet()
{
	bFiring = false;
}

void UTracerComponent::InterpolateItem()
{
	if (MainCharacter && TraceHitItem)
	{
		TraceHitItem->StartItemCurve(MainCharacter);
		TraceHitItem->PlayPickupSoundCue();
		// we set it to nullptr again, because otherwise it can call StartItemCurrve repeatedly.
		TraceHitItem = nullptr;
		HeldItem = nullptr;
	}
}

FVector UTracerComponent::GetBeamEndPoint()
{
	if (MainCharacter && MainCharacter->GetEquippedWeapon())
	{
		FVector CrosshairWorldPosition;
		FVector CrosshairWorldDirection;
		
		bool bScreenToWorld = IsConvertedScreenToWorld(CrosshairWorldPosition, CrosshairWorldDirection);
	
		if (bScreenToWorld)
		{
			FTransform BarrelSocketTransform = MainCharacter->GetEquippedWeapon()->GetBarrelSocketTransform();
		
			LineTraceFromTheScreen(CrosshairWorldPosition, CrosshairWorldDirection, BeamEnd);
			LineTraceFromTheGunBarrel(BarrelSocketTransform.GetLocation(), BeamEnd);
			CrosshairStartFireBullet();
		}
		GEngine->AddOnScreenDebugMessage(2, 2, FColor::Cyan, FString::Printf(TEXT("Beam End : %f, %f, %f"), BeamEnd.X, BeamEnd.Y, BeamEnd.Z));

		return BeamEnd;
	}

	return FVector::ZeroVector;
}

void UTracerComponent::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		SetShouldTraceItem(false);			
		OverlappedItemCount = 0;
	}

	else
	{
		SetShouldTraceItem(true);
		OverlappedItemCount += Amount;
	}
}