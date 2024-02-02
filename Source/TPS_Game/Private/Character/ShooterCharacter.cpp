// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AnimInstance/ShooterAnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/TracerComponent.h"
#include "Components/CrosshairAnimatorComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/Ammo.h"
#include "Item/BaseItem.h"
#include "Item/Weapon.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AShooterCharacter::AShooterCharacter() :
	// Turn Rates for aiming/not aiming
	HipTurnAndLookUpRates(1.f, 1.f),
	AimingTurnAndLookUpRates(0.5f, 0.5f),
	// Automatic Gun Fire Factors
	AutomaticFireRate(0.1f),
	bFireButtonPressed(false),
	// Camera Field Of View values to use aiming/not aiming
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(25.f),
	CameraCurrentFOV(0.f),
	CameraZoomInterpSpeed(25.f),
	OverlappedItemCount(0),
	CameraForwardDistance(120.f),
	CameraUpDistance(30.f),
	bAiming(false),
	Starting9mmAmmo(90),
	StartingARAmmo(120),
	CombatState(ECombatState::ECS_Unoccupied),
	bIsCrouching(false),
	RunningSpeed(650.f),
	CrouchingSpeed(300.f),
	RunningHalfHeight(88.f),
	CrouchingHalfHeight(50.f)
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 35.f, 70.f);
	
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
	GetCharacterMovement()->MaxAcceleration = 1750.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 315.f;
	GetCharacterMovement()->GroundFriction = 3.f;

	LeftHandSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("LeftHandSceneComponent"));
	CrosshairAnimatorComponent = CreateDefaultSubobject<UCrosshairAnimatorComponent>(TEXT("CrosshairAnimatorComponent"));
	TracerComponent = CreateDefaultSubobject<UTracerComponent>(TEXT("TracerComponent"));

	WeaponInterpTargetComp = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponInterpTarget"));
	WeaponInterpTargetComp->SetupAttachment(FollowCamera);
	ItemInterpTargetComp1 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemInterpTargetComp1"));
	ItemInterpTargetComp1->SetupAttachment(FollowCamera);
	ItemInterpTargetComp2 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemInterpTargetComp2"));
	ItemInterpTargetComp2->SetupAttachment(FollowCamera);
	ItemInterpTargetComp3 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemInterpTargetComp3"));
	ItemInterpTargetComp3->SetupAttachment(FollowCamera);
	ItemInterpTargetComp4 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemInterpTargetComp4"));
	ItemInterpTargetComp4->SetupAttachment(FollowCamera);
	ItemInterpTargetComp5 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemInterpTargetComp5"));
	ItemInterpTargetComp5->SetupAttachment(FollowCamera);
	ItemInterpTargetComp6 = CreateDefaultSubobject<USceneComponent>(TEXT("ItemInterpTargetComp6"));
	ItemInterpTargetComp6->SetupAttachment(FollowCamera);
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (!TracerComponent)
	{
		GEngine->AddOnScreenDebugMessage(1, 2, FColor::Cyan, FString::Printf(TEXT("Combat is not valid")));
	}

	if (ShooterAnimInstance == nullptr)
	{
		TObjectPtr<UAnimInstance> AnimInstance = GetMesh()->GetAnimInstance();
		ShooterAnimInstance = Cast<UShooterAnimInstance>(AnimInstance);
	}

	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	if (EquippedWeapon)
	{
		EquippedWeapon->SetGlowMaterialEnabled(1.f);
	}
	
	InitializeAmmoMap();
	// Set the animation BP class of character mesh
	GetMesh()->SetAnimClass(AnimationClass);
	EquipWeapon(SpawnDefaultWeapon());
	
	TObjectPtr<APlayerController> PlayerController = Cast<APlayerController>(GetController());

	if (PlayerController)
	{
		TObjectPtr<UEnhancedInputLocalPlayerSubsystem> Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());

		if (Subsystem)
		{
			Subsystem->AddMappingContext(ShooterInputMapping, 0);
		}
	}
	
	InitializeInterpLocationContainer();
}

void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	CameraInterpZoom(DeltaTime);
	UpdateCapsuleHalfHeight(DeltaTime);
	SetLookRates();
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
			EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this,  &AShooterCharacter::Jump);
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

		if (ReloadAction)
		{
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AShooterCharacter::ReloadButtonPressed);
		}

		if (CrouchingAction)
		{
			EnhancedInputComponent->BindAction(CrouchingAction, ETriggerEvent::Triggered, this, &AShooterCharacter::CrouchingButtonPressed);
			EnhancedInputComponent->BindAction(CrouchingAction, ETriggerEvent::Completed, this, &AShooterCharacter::CrouchingButtonReleased);
		}
	}
}

void AShooterCharacter::Jump()
{
	if (!bIsCrouching)
	{
		Super::Jump();
	}
}

void AShooterCharacter::Movement(const FInputActionValue& Value)
{
	if (!GetCharacterMovement()->IsFalling())
	{
		/** Movement Direction Vector comes from the keyboard */
		const FVector2D MovementDirection = Value.Get<FVector2D>();
		/** Controller Rotation which is chanhing by the mouse or etc. */
		const FRotator ControllerRotation = GetControlRotation();
		/** Yaw component of the controller rotation value. */
		const FRotator YawRotation = FRotator(0.f, ControllerRotation.Yaw, 0.f);
		/** Set forward vector by using the rotation matrix. */
		const FVector ForwardVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardVector, MovementDirection.Y);
		/** Set right vector by using the rotation matrix. */
		const FVector RightVector = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightVector, MovementDirection.X);	
	}
}

void AShooterCharacter::LookAround(const FInputActionValue& Value)
{
	FVector2D LookDirection = Value.Get<FVector2D>();
	
	AddControllerYawInput(LookDirection.X * BaseTurnAndLoopUpRates.X);
	AddControllerPitchInput(LookDirection.Y * BaseTurnAndLoopUpRates.Y);
}	

void AShooterCharacter::Fire()
{
	if (EquippedWeapon &&
			CombatState == ECombatState::ECS_Unoccupied && TracerComponent)
	{
		FVector BeamEndPoint = TracerComponent->GetBeamEndPoint();
		FTransform BarrelSocketTransform = EquippedWeapon->GetBarrelSocketTransform();
		PlayGunFireMontage();
		PlayHitParticle(BeamEndPoint);
		PlayBeamParticle(BarrelSocketTransform, BeamEndPoint);
		PlayFireSoundCue();
		PlayBarrelMuzzleFlash();
		EquippedWeapon->DecremenetAmmo();
		StartFireTimer();
	}
}

void AShooterCharacter::OpenScope(const FInputActionValue& Value)
{
	bIsScopeOpen = Value.Get<bool>();

	if (bIsScopeOpen && CombatState != ECombatState::ECS_Reloading)
	{
		StartAim();
	}
}

void AShooterCharacter::CloseScope(const FInputActionValue& Value)
{
	bIsScopeOpen = Value.Get<bool>();
	StopAim();	
}

void AShooterCharacter::SelectButtonPressed(const FInputActionValue& Value)
{
	bool bIsPressed = Value.Get<bool>();

	if (bIsPressed && TracerComponent)
	{
		TracerComponent->PickUpItem();
	}
}

void AShooterCharacter::ReloadButtonPressed(const FInputActionValue& Value)
{
	bool bIsReloaded = Value.Get<bool>();
	if (bIsReloaded)
	{
		ReloadWeapon();
	}
}

void AShooterCharacter::CrouchingButtonPressed(const FInputActionValue& Value)
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bool bCrouchingPressed = Value.Get<bool>();

		bIsCrouching = bCrouchingPressed;
		GetCharacterMovement()->MaxWalkSpeed = CrouchingSpeed;
		GetCharacterMovement()->GroundFriction = 5.f;
	}
}

void AShooterCharacter::CrouchingButtonReleased(const FInputActionValue& Value)
{
	if (!GetCharacterMovement()->IsFalling())
	{
		bool bCrouchingReleased = Value.Get<bool>();

		bIsCrouching = bCrouchingReleased;
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		GetCharacterMovement()->GroundFriction = .75f;	
	}
}

void AShooterCharacter::FireButtonPressed(const FInputActionValue& Value)
{
	bFireButtonPressed = Value.Get<bool>();

	if (bFireButtonPressed)
	{
		Fire();
	}
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
	if (EquippedWeapon)
	{
		if (EquippedWeapon->GetCurrentAmmo() > 0)
		{
			if (bFireButtonPressed)
			{
				Fire();
			}	
		}

		else
		{
			ReloadWeapon();
		}	
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
	if (WeaponToEquip)
	{
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
}

void AShooterCharacter::DropWeapon()
{
	if (EquippedWeapon)
	{
		FDetachmentTransformRules DetachmentTransformRules(EDetachmentRule::KeepWorld, true);
		EquippedWeapon->GetItemMesh()->DetachFromComponent(DetachmentTransformRules);
		EquippedWeapon->SetItemState(EItemState::EIS_Falling);
		EquippedWeapon->ThrowWeapon();
	}
}

void AShooterCharacter::SwapWeapon(TObjectPtr<AWeapon> WeaponToSwap)
{
	if (WeaponToSwap)
	{
		DropWeapon();
		EquipWeapon(WeaponToSwap);
	}
}

void AShooterCharacter::ReloadWeapon()
{
	if (CombatState == ECombatState::ECS_Unoccupied && CarryingAmmo() && !EquippedWeapon->IsMagazineFull())
	{
		if (bAiming)
		{
			StopAim();
		}
		CombatState = ECombatState::ECS_Reloading;
		PlayReloadWeaponMontage();
	}
}

bool AShooterCharacter::CarryingAmmo()
{
	if (EquippedWeapon)
	{
		EAmmoType AmmoType = EquippedWeapon->GetAmmoType();

		if (AmmoMap.Contains(AmmoType))
		{
			/** return true if we have ammo corresponding to the AmmoType */
			return AmmoMap[AmmoType] > 0;
		}
		return false;	
	}
	return false;
}

void AShooterCharacter::UpdateCapsuleHalfHeight(float DeltaTime)
{
	bIsCrouching ? TargetHalfHeight = CrouchingHalfHeight : TargetHalfHeight = RunningHalfHeight;

	const float InterpHalfHeight { FMath::FInterpTo(
		GetCapsuleComponent()->GetScaledCapsuleHalfHeight(),
		TargetHalfHeight,
		DeltaTime,
		20.f)};

	const float DeltaCapsuleHalfHeight { InterpHalfHeight - GetCapsuleComponent()->GetScaledCapsuleHalfHeight() };
	const FVector MeshOffset { 0.f, 0.f, -DeltaCapsuleHalfHeight};
	GetMesh()->AddLocalOffset(MeshOffset);
	
	GetCapsuleComponent()->SetCapsuleHalfHeight(InterpHalfHeight);
}

int32 AShooterCharacter::GetAmmoCountByWeaponType()
{
	if (EquippedWeapon)
	{
		const auto AmmoType = EquippedWeapon->GetAmmoType();
		if (AmmoMap.Contains(AmmoType))
		{
			return AmmoMap[AmmoType];
		}	
	}
	return -1;
}

FInterpLocation AShooterCharacter::GetInterpLocation(int32 Index)
{
	if (Index < ItemInterpLocations.Num() && Index >= 0)
	{
		return ItemInterpLocations[Index];
	}
	return FInterpLocation();
}

void AShooterCharacter::FinishReloading()
{
	CombatState = ECombatState::ECS_Unoccupied;

	if (bIsScopeOpen)
	{
		StartAim();
	}

	if (EquippedWeapon)
	{
		const EAmmoType AmmoType = EquippedWeapon->GetAmmoType();

		if (AmmoMap.Contains(AmmoType))
		{
			// Returns to us useable ammo amount of that AmmoType
			int32 CarriedAmmo = AmmoMap[AmmoType];
			// Space left in the magazine of EquippedWeapon
			const int32 MagazineEmptySpace = EquippedWeapon->GetMagazineCapacity() - EquippedWeapon->GetCurrentAmmo();

			if (MagazineEmptySpace > CarriedAmmo)
			{
				// Reload the weapon with carried ammo
				EquippedWeapon->ReloadAmmo(CarriedAmmo);
				CarriedAmmo = 0;
				AmmoMap.Add(AmmoType, CarriedAmmo);
			}

			else
			{
				// Fill the weapon
				EquippedWeapon->ReloadAmmo(MagazineEmptySpace);
				CarriedAmmo -= MagazineEmptySpace;
				AmmoMap.Add(AmmoType, CarriedAmmo);
			}
		}	
	}
}

void AShooterCharacter::GrabMagazine()
{
	if (EquippedWeapon == nullptr) { return; }
	if (EquippedWeapon->GetItemMesh() == nullptr) {return;}
	if (LeftHandSceneComponent == nullptr) { return; }
	
	FName MagazineName = EquippedWeapon->GetMagazineBoneName();
	int32 MagazineIndex = EquippedWeapon->GetItemMesh()->GetBoneIndex(MagazineName);
	MagazineTransform = EquippedWeapon->GetItemMesh()->GetBoneTransform(MagazineIndex);
	/** KeepRelative does the attached component hold its position by getting parent object as a center point. */
	FAttachmentTransformRules AttachmentTransformRules(EAttachmentRule::KeepRelative, true);
	// SceneComponent attached to the character mesh socket via the socket.
	LeftHandSceneComponent->AttachToComponent(GetMesh(), AttachmentTransformRules, FName(TEXT("hand_l")));
	/** when leftHandSceneComponent is attached to the hand_l bone, leftHandSceneComponent center is the bone center.
	 * But when character holds the weapon magazine, magazine location and rotation is updated by using of leftHandSceneComp transform.
	 * In this process, leftHandSceneComponent is the center of hand_l bone and because of that, magazine looks like overlapping with
	 * mesh. To fix this, at the time of grabbing magazine, we update component transform as magazine transform. Because when character close its hand
	 * and grab magazine, magazine looks like the center of the closed hand. */
	LeftHandSceneComponent->SetWorldTransform(MagazineTransform);
	EquippedWeapon->SetbIsMovingMagazine(true);
}

void AShooterCharacter::ReplaceMagazine()
{
	EquippedWeapon->SetbIsMovingMagazine(false);
}

void AShooterCharacter::GetPickUpItem(TObjectPtr<ABaseItem> PickedUpItem)
{
	TObjectPtr<AWeapon> PickedUpWeapon = Cast<AWeapon>(PickedUpItem);

	if (PickedUpWeapon)
	{
		SwapWeapon(PickedUpWeapon);
	}

	TObjectPtr<AAmmo> PickedUpAmmo = Cast<AAmmo>(PickedUpItem);
	if (PickedUpAmmo)
	{
		PickUpAmmo(PickedUpAmmo);
	}
}

void AShooterCharacter::PickUpAmmo(TObjectPtr<AAmmo> PickedUpAmmo)
{
	if (AmmoMap.Contains(PickedUpAmmo->GetAmmoType()))
	{
		// Get the existing ammo count in the map correspond to the ammo type
		int32 ExistingAmmoCount = AmmoMap[PickedUpAmmo->GetAmmoType()];
		// Add the picked up ammo count to the existing
		ExistingAmmoCount += PickedUpAmmo->GetAmmoCount();
		// Set the new value to the value corresponding to the ammo type on the map
		AmmoMap[PickedUpAmmo->GetAmmoType()] = ExistingAmmoCount;
	}
	// Autamatic Reloading the weapon when it is empty
	if (EquippedWeapon->GetAmmoType() == PickedUpAmmo->GetAmmoType() && EquippedWeapon->GetCurrentAmmo() == 0)
	{
		ReloadWeapon();
	}
	PickedUpAmmo->Destroy();
}

void AShooterCharacter::InitializeInterpLocationContainer()
{
	/** Weapon Item interp location */
	FInterpLocation WeaponLocation = {WeaponInterpTargetComp, 0};
	ItemInterpLocations.Add(WeaponLocation);
	/** Non-Weapon Item interp locations */
	FInterpLocation ItemInterpLocation1 = {ItemInterpTargetComp1, 0};
	ItemInterpLocations.Add(ItemInterpLocation1);
	FInterpLocation ItemInterpLocation2 = {ItemInterpTargetComp2, 0};
	ItemInterpLocations.Add(ItemInterpLocation2);
	FInterpLocation ItemInterpLocation3 = {ItemInterpTargetComp3, 0};
	ItemInterpLocations.Add(ItemInterpLocation3);
	FInterpLocation ItemInterpLocation4 = {ItemInterpTargetComp4, 0};
	ItemInterpLocations.Add(ItemInterpLocation4);
	FInterpLocation ItemInterpLocation5 = {ItemInterpTargetComp5, 0};
	ItemInterpLocations.Add(ItemInterpLocation5);
	FInterpLocation ItemInterpLocation6 = {ItemInterpTargetComp6, 0};
	ItemInterpLocations.Add(ItemInterpLocation6);
}

void AShooterCharacter::UpdateInterpingItemCount(int32 Index, int32 Amount)
{
	/** Amount should be in range of [-1, 1] */
	if (Amount < -1 || Amount > 1) { return; }

	if (Index < ItemInterpLocations.Num() && Index >= 0)
	{
		ItemInterpLocations[Index].InterpingItemCount += Amount;
	}
}

int32 AShooterCharacter::GetInterpLocationIndex()
{
	int32 LowestCountIndex = 1;
	int32 LowestCount = INT_MAX;

	for (int i = 1; i < ItemInterpLocations.Num(); i++)
	{
		if (LowestCount > ItemInterpLocations[i].InterpingItemCount)
		{
			LowestCount = ItemInterpLocations[i].InterpingItemCount;
			LowestCountIndex = i;
		}
	}
	
	return LowestCountIndex;
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
		if (TracerComponent)
		{
			TracerComponent->SetShouldTraceItem(false);			
		}
		OverlappedItemCount = 0;
		UE_LOG(LogTemp, Warning, TEXT("Increased"));
	}

	else
	{
		if (TracerComponent)
		{
			TracerComponent->SetShouldTraceItem(true);			
		}
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

void AShooterCharacter::SetLookRates()
{
	bAiming ? BaseTurnAndLoopUpRates = AimingTurnAndLookUpRates : BaseTurnAndLoopUpRates = HipTurnAndLookUpRates;
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

	if (GetFollowCamera())
	{
		GetFollowCamera()->SetFieldOfView(CameraCurrentFOV);
	}
}
void AShooterCharacter::PlayGunFireMontage()
{
	if (ShooterAnimInstance && GunFireMontage)
	{
		ShooterAnimInstance->Montage_Play(GunFireMontage);
		ShooterAnimInstance->Montage_JumpToSection(FName("StartFire"));
	}
}

void AShooterCharacter::PlayReloadWeaponMontage()
{
	if (ShooterAnimInstance && ReloadMontage && EquippedWeapon)
	{
		ShooterAnimInstance->Montage_Play(ReloadMontage);
		ShooterAnimInstance->Montage_JumpToSection(EquippedWeapon->GetReloadingWeaponSection());
	}
}

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

void AShooterCharacter::StartAim()
{
	bAiming = true;
	GetCharacterMovement()->MaxWalkSpeed = CrouchingSpeed;
}

void AShooterCharacter::StopAim()
{
	bAiming = false;
	if (!bIsCrouching)
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	}
}