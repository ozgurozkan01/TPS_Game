// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/BaseItem.h"
#include "Item/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"

AShooterCharacter::AShooterCharacter() :
	// Turn Rates for aiming/not aiming 
	HipTurnRate(1.f),
	HipLookUpRate(1.f),
	AimingTurnRate(0.2f),
	AimingLookUpRate(0.2f),
	// Automatic Gun Fire Factors
	AutomaticFireRate(0.1f),
	bFireButtonPressed(false),
	// Camera Field Of View values to use aiming/not aiming
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(25.f),
	CameraCurrentFOV(0.f),
	CameraZoomInterpSpeed(25.f),
	// Crosshair spread factors
	CrosshairSpreadMultiplier(0.f),
	CrosshairInAirMultiplier(0.f),
	CrosshairAimingMultiplier(0.f),
	CrosshairShootingMultiplier(0.f),
	// Crosshair max spread value
	CrosshairSpreadMax(16.f),
	// Crosshair Fire Factors
	FireBulletDuration(0.05f),
	bFiring(false),
	bShouldTraceForItems(false),
	OverlappedItemCount(0),
	CameraForwardDistance(120.f),
	CameraUpDistance(30.f),
	bAiming(false),
	Starting9mmAmmo(90),
	StartingARAmmo(120),
	CombatState(ECombatState::ECS_Unoccupied)
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 70.f);
	
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;
	
	// Set the animation mode of character mesh
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);

	// deactivate the character rotate with controller
	// Do not rotate when the controller rotates
	// Let the controller only affect the camera
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Activate the character rotate towards movement vector.
	GetCharacterMovement()->bOrientRotationToMovement = false; // Player rotates the movement direction
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540, 0.f);
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.15f; // Control percent when the character is in air
	GetCharacterMovement()->MaxAcceleration = 1250.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 315.f;
	GetCharacterMovement()->GroundFriction = 3.f;
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitializeAmmoMap();
	
	// Set the animation BP class of character mesh
	GetMesh()->SetAnimClass(AnimationClass);
	EquipWeapon(SpawnDefaultWeapon());
	
	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}
	
	TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (Subsystem)
		{
			Subsystem->AddMappingContext(ShooterInputMapping, 0);
		}
	}
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CameraInterpZoom(DeltaTime);
	SetLookRates();
	CalculateCrosshairSpreadMultiplier(DeltaTime);
	LineTraceForInformationPopUp();
}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	TObjectPtr<UEnhancedInputComponent> EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (EnhancedInputComponent)
	{
		if (MovementAction)
		{
			EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this,  &AShooterCharacter::Movement);
		}

		if (LookAction)
		{
			EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,  &AShooterCharacter::LookAround);
		}
		
		if (JumpAction)
		{
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this,  &ACharacter::Jump);
		}

		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AShooterCharacter::FireButtonPressed);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AShooterCharacter::FireButtonReleased);
		}

		if (ScopeAction)
		{
			EnhancedInputComponent->BindAction(ScopeAction, ETriggerEvent::Triggered, this, &AShooterCharacter::OpenScope);
			EnhancedInputComponent->BindAction(ScopeAction, ETriggerEvent::Completed, this, &AShooterCharacter::CloseScope);
		}

		if (SelectAction)
		{
			EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &AShooterCharacter::SelectButtonPressed);
		}

		/*if (ReloadAction)
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AShooterCharacter::ReloadButtonPressed);
		}*/
	}
}

void AShooterCharacter::Movement(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->IsFalling()) { return; }
	
	const FVector2D MovementDirection = Value.Get<FVector2D>();

	const FRotator ControllerRotation = GetControlRotation();
	const FRotator YawRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);

	const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	AddMovementInput(ForwardVector, MovementDirection.Y);
	const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	AddMovementInput(RightVector, MovementDirection.X);
}

void AShooterCharacter::LookAround(const FInputActionValue& Value)
{
	FVector2D LookDirection = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookDirection.X * BaseTurnRate);
	AddControllerPitchInput(LookDirection.Y * BaseLookUpRate);
}	

void AShooterCharacter::Fire()
{
	if (EquippedWeapon == nullptr ||
			(EquippedWeapon && !EquippedWeapon->HasAmmo()) ||
			CombatState != ECombatState::ECS_Unoccupied)
	{ return; }

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	bool bScreenToWorld = IsConvertedScreenToWorld(CrosshairWorldPosition, CrosshairWorldDirection);
	
	if (bScreenToWorld)
	{
		FTransform BarrelSocketTransform = EquippedWeapon->GetBarrelSocketTransform();
		
		FVector BeamEndPoint { FVector::ZeroVector };
		LineTraceFromTheScreen(CrosshairWorldPosition, CrosshairWorldDirection, BeamEndPoint);
		LineTraceFromTheGunBarrel(BarrelSocketTransform.GetLocation(), BeamEndPoint);

		PlayGunFireMontage();
		PlayHitParticle(BeamEndPoint);
		PlayBeamParticle(BarrelSocketTransform, BeamEndPoint);
		PlayFireSoundCue();
		PlayBarrelMuzzleFlash();
		CrosshairStartFireBullet();

		EquippedWeapon->DecremenetAmmo();
		StartFireTimer();
	}
}

void AShooterCharacter::OpenScope(const FInputActionValue& Value)
{
	bool bCrosshairShouldZoom = Value.Get<bool>();

	bAiming = bCrosshairShouldZoom;
}

void AShooterCharacter::CloseScope(const FInputActionValue& Value)
{
	bool bCrosshairShouldZoom = Value.Get<bool>();
	
	bAiming = bCrosshairShouldZoom;
}

void AShooterCharacter::SelectButtonPressed(const FInputActionValue& Value)
{
	bool bIsPressed = Value.Get<bool>();

	if (TraceHitItem && bIsPressed)
	{
		TraceHitItem->StartItemCurve(this);
		// we set it to nullptr again, because otherwise it can call StartItemCurrve repeatedly.
		TraceHitItem = nullptr;
	}
}

/*void AShooterCharacter::ReloadButtonPressed(const FInputActionValue& Value)
{
	bool bIsReloaded = Value.Get<bool>();
	if (bIsReloaded)
	{
		ReloadWeapon();
	}
}*/

bool AShooterCharacter::IsConvertedScreenToWorld(FVector& CrosshairWorldPosition, FVector& CrosshairWorldDirection)
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

void AShooterCharacter::LineTraceFromTheScreen(const FVector& CrosshairWorldPosition, const FVector& CrosshairWorldDirection, FVector& BeamEndPoint)
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

void AShooterCharacter::LineTraceFromTheGunBarrel(const FVector& GunSocketLocation, FVector& BeamEndPoint)
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

void AShooterCharacter::LineTraceForInformationPopUp()
{
	if (!bShouldTraceForItems)
	{
		if (HeldItem)
		{
			HeldItem->GetInformationWidgetComponent()->SetVisibility(false);
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

			if (TraceHitItem && TraceHitItem->GetInformationWidgetComponent())
			{	
				TraceHitItem->GetInformationWidgetComponent()->SetVisibility(true);
			}

			if (HeldItem && HeldItem != TraceHitItem)
			{
				HeldItem->GetInformationWidgetComponent()->SetVisibility(false);
			}
			
			HeldItem = TraceHitItem;
		}
	}
	
	else if (HeldItem)
	{
		HeldItem->GetInformationWidgetComponent()->SetVisibility(false);
	}
}

void AShooterCharacter::PlayGunFireMontage()
{
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && GunFireMontage)
	{
		AnimInstance->Montage_Play(GunFireMontage);
		AnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

/*void AShooterCharacter::PlayReloadWeaponMontage()
{
	TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();

	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		AnimInstance->Montage_JumpToSection(FName("Reload SMG"));
	}
}*/

void AShooterCharacter::PlayHitParticle(const FVector& HitLocation)
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitLocation);
	}
}

void AShooterCharacter::PlayBeamParticle(const FTransform& Start, const FVector& End)
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

void AShooterCharacter::PlayFireSoundCue()
{
	if (FireSoundCue)
	{
		UGameplayStatics::PlaySound2D(this, FireSoundCue);
	}
}

void AShooterCharacter::PlayBarrelMuzzleFlash()
{
	if (MuzzleFlash && EquippedWeapon)
	{
		FTransform BarrelSocketTransform = EquippedWeapon->GetBarrelSocketTransform();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BarrelSocketTransform);
	}
}

void AShooterCharacter::CrosshairStartFireBullet()
{
	bFiring = true;

	GetWorldTimerManager().SetTimer(FireBulletHandle, this, &AShooterCharacter::CrosshairFinishFireBullet, FireBulletDuration);
}

void AShooterCharacter::CrosshairFinishFireBullet()
{
	bFiring = false;
}

float AShooterCharacter::InterpCurrentFOV(float TargetFOV, float DeltaTime)
{
	return FMath::FInterpTo(CameraCurrentFOV, TargetFOV, DeltaTime, CameraZoomInterpSpeed);
}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
	if (bAiming)
	{
		CameraCurrentFOV = InterpCurrentFOV(CameraZoomedFOV, DeltaTime);
	}

	else
	{
		CameraCurrentFOV = InterpCurrentFOV(CameraDefaultFOV, DeltaTime);
	}

	if (FollowCamera)
	{
		GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	}
}

void AShooterCharacter::SetLookRates()
{
	if(bAiming)
	{
		BaseTurnRate = AimingTurnRate;
		BaseLookUpRate = AimingLookUpRate;
	}

	else
	{
		BaseTurnRate = HipTurnRate;
		BaseLookUpRate = HipLookUpRate;
	}
}

void AShooterCharacter::CalculateCrosshairSpreadMultiplier(float DeltaTime)
{
	CrosshairSpreadMultiplier =
		0.5 +
		CalculateCrosshairVelocityMultiplier() +
		CalculateCrosshairInAirMultiplier(DeltaTime) +
		CalculateCrosshairFireAimingMultiplier(DeltaTime) -
		CalculateCrosshairAimingMultiplier(DeltaTime);
}

float AShooterCharacter::CalculateCrosshairVelocityMultiplier()
{
	FVector2D WalkSpeedRange {0.f, 600.f};
	FVector2D VelocityMultiplierRange {0.f, 1.f};
	FVector CurrentVelocity = GetVelocity();
	CurrentVelocity.Z = 0.f;

	/** Clamp the speed value based on output range value and return corresponding percentage accordingly output*/
	return FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, CurrentVelocity.Size());	
}

float AShooterCharacter::CalculateCrosshairInAirMultiplier(float DeltaTime)
{
	if (GetCharacterMovement()->IsFalling())
	{
		CrosshairInAirMultiplier = FMath::FInterpTo(CrosshairInAirMultiplier, 2.25f, DeltaTime, 6.f); 
	}

	else
	{
		CrosshairInAirMultiplier = FMath::FInterpTo(CrosshairInAirMultiplier, 0, DeltaTime, 10.f); 
	}

	return CrosshairInAirMultiplier;
}

float AShooterCharacter::CalculateCrosshairAimingMultiplier(float DeltaTime)
{
	if (bAiming)
	{
		CrosshairAimingMultiplier = FMath::FInterpTo(CrosshairAimingMultiplier, 0.35f, DeltaTime, 6.f); 
	}

	else
	{
		CrosshairAimingMultiplier = FMath::FInterpTo(CrosshairAimingMultiplier, 0, DeltaTime, 10.f); 
	}

	return CrosshairAimingMultiplier;
	
}

float AShooterCharacter::CalculateCrosshairFireAimingMultiplier(float DeltaTime)
{
	if (bFiring)
	{
		CrosshairShootingMultiplier = FMath::FInterpTo(CrosshairShootingMultiplier, 0.2f, DeltaTime, 45.f); 
	}

	else
	{
		CrosshairShootingMultiplier = FMath::FInterpTo(CrosshairShootingMultiplier, 0.f, DeltaTime, 45.f); 
	}

	return CrosshairShootingMultiplier;
}

void AShooterCharacter::FireButtonPressed(const FInputActionValue& Value)
{
	bFireButtonPressed = Value.Get<bool>();

	if (!bFireButtonPressed) return;

	Fire();
}

void AShooterCharacter::FireButtonReleased(const FInputActionValue& Value)
{
	bFireButtonPressed = false;
}

void AShooterCharacter::StartFireTimer()
{
	CombatState = ECombatState::ECS_FireTimerInProgress;
	GetWorldTimerManager().SetTimer(AutomaticFireHandle, this, &AShooterCharacter::AutomaticFireReset, AutomaticFireRate);
}

void AShooterCharacter::AutomaticFireReset()
{
	CombatState = ECombatState::ECS_Unoccupied;
	
	if (EquippedWeapon && EquippedWeapon->HasAmmo())
	{
		if (bFireButtonPressed)
		{
			Fire();
		}	
	}

	else
	{
		//ReloadWeapon();
	}
}

TObjectPtr<AWeapon> AShooterCharacter::SpawnDefaultWeapon()
{
	if (DefaultWeaponClass)
	{
		TObjectPtr<AWeapon> DefaultWeapon = GetWorld()->SpawnActor<AWeapon>(DefaultWeaponClass);
		return DefaultWeapon;
	}

	return nullptr;
}

void AShooterCharacter::EquipWeapon(TObjectPtr<AWeapon> WeaponToEquip)
{
	if (WeaponToEquip == nullptr) { return; }
	
	const USkeletalMeshSocket* HandSocket = GetMesh()->GetSocketByName(FName("RightHandSocket"));

	if (HandSocket)
	{
		HandSocket->AttachActor(WeaponToEquip, GetMesh());
		WeaponToEquip->SetIdleMovement(false);
		WeaponToEquip->SetItemCollisions(false);
	}

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetItemState(EItemState::EIS_Equipped);
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon == nullptr) { return; }

	FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
	EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
	EquippedWeapon->SetItemState(EItemState::EIS_Falling);
	EquippedWeapon->ThrowWeapon();
}

void AShooterCharacter::SwapWeapon(TObjectPtr<AWeapon> WeaponToSwap)
{
	if (WeaponToSwap)
	{
		DropWeapon();
		EquipWeapon(WeaponToSwap);
	}
}

/*void AShooterCharacter::ReloadWeapon()
{
	if (CombatState != ECombatState::ECS_Unoccupied) { return; }
	
	if (EquippedWeapon && !EquippedWeapon->IsMagazineFull() && Starting9mmAmmo > 0)
	{
		CombatState = ECombatState::ECS_Reloading;
		EquippedWeapon->SetReloadedAmmo(Starting9mmAmmo);
		PlayReloadWeaponMontage();
	}
}*/

void AShooterCharacter::FinishReloading()
{
	CombatState = ECombatState::ECS_Unoccupied;
}

void AShooterCharacter::GetPickUpItem(TObjectPtr<ABaseItem> PickedUpItem)
{
	TObjectPtr<AWeapon> PickedUpWeapon = Cast<AWeapon>(PickedUpItem);

	if (PickedUpWeapon)
	{
		SwapWeapon(PickedUpWeapon);
	}
}

void AShooterCharacter::InitializeAmmoMap()
{
	AmmoMap.Add(EAmmoType::EAT_9mm, Starting9mmAmmo);
	AmmoMap.Add(EAmmoType::EAT_AR, StartingARAmmo);
}

void AShooterCharacter::IncrementOverlappedItemCount(int8 Amount)
{
	if (OverlappedItemCount + Amount <= 0)
	{
		bShouldTraceForItems = false;
		OverlappedItemCount = 0;
		UE_LOG(LogTemp, Warning, TEXT("Increased"));
	}

	else
	{
		bShouldTraceForItems = true;
		OverlappedItemCount += Amount;
		UE_LOG(LogTemp, Warning, TEXT("Decreased"));
	}
}

FVector AShooterCharacter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation { FollowCamera->GetComponentLocation() };
	const FVector CameraForwardVector { FollowCamera->GetForwardVector() };

	return CameraWorldLocation + CameraForwardVector * CameraForwardDistance + FVector::UpVector * CameraUpDistance;
}
