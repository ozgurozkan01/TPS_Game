// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/ShooterCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"

AShooterCharacter::AShooterCharacter() :
	bAiming(false),
	// Turn Rates for aiming/not aiming 
	HipTurnRate(1.f),
	HipLookUpRate(1.f),
	AimingTurnRate(0.2f),
	AimingLookUpRate(0.2f),
	// Camera Field Of View values to use aiming/not aiming
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(25.f),
	CameraCurrentFOV(0.f),
	CameraZoomInterpSpeed(25.f),
	// Crosshair max spread value
	CrosshairSpreadMax(16.f),
	CrosshairInAirMultiplier(0.f),
	CrosshairAimingMultiplier(0.f)
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

	// Set the animation BP class of character mesh
	GetMesh()->SetAnimClass(AnimationClass);
	
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
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AShooterCharacter::Fire);
		}

		if (ScopeAction)
		{
			EnhancedInputComponent->BindAction(ScopeAction, ETriggerEvent::Triggered, this, &AShooterCharacter::OpenScope);
			EnhancedInputComponent->BindAction(ScopeAction, ETriggerEvent::Completed, this, &AShooterCharacter::CloseScope);
		}
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

void AShooterCharacter::Fire(const FInputActionValue& Value)
{
	PlayGunFireMontage();
	Shoot();
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

FTransform AShooterCharacter::GetGunBarrelSocketTransform()
{
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");

	if (BarrelSocket)
	{
		FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		return SocketTransform;
	}

	return FTransform();
}

bool AShooterCharacter::IsConvertedScreenToWorld(FVector& CrosshairWorldPosition, FVector& CrosshairWorldDirection)
{
	FVector2D ViewportSize;

	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}

	FVector2D CrosshairPosition = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);
	CrosshairPosition.Y -= 50.f;
	
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

	//DrawDebugLine(GetWorld(), WeaponTraceStart, BeamEndPoint, FColor::Red, false, 5.f);
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

void AShooterCharacter::PlayFireSoundCue()
{
	if (FireSoundCue)
	{
		UGameplayStatics::PlaySound2D(this, FireSoundCue);
	}
}

void AShooterCharacter::PlayBarrelMuzzleFlash()
{
	if (MuzzleFlash)
	{
		FTransform BarrelSocketTransform = GetGunBarrelSocketTransform();
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, BarrelSocketTransform);
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

void AShooterCharacter::Shoot()
{
	FTransform BarrelSocketTransform = GetGunBarrelSocketTransform();

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	if (IsConvertedScreenToWorld(CrosshairWorldPosition, CrosshairWorldDirection))
	{
		FVector BeamEndPoint { FVector::ZeroVector };
		LineTraceFromTheScreen(CrosshairWorldPosition, CrosshairWorldDirection, BeamEndPoint);
		LineTraceFromTheGunBarrel(BarrelSocketTransform.GetLocation(), BeamEndPoint);
		
		PlayHitParticle(BeamEndPoint);
		PlayBeamParticle(BarrelSocketTransform, BeamEndPoint);
		PlayFireSoundCue();
	}
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
	CrosshairSpreadMultiplier = 0.5 + CalculateCrosshairVelocityMultiplier() + CalculateCrosshairInAirMultiplier(DeltaTime) - CalculateCrosshairAimingMultiplier(DeltaTime);
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

float AShooterCharacter::GetCrosshairSpreadValue()
{
	return CrosshairSpreadMultiplier;
}

float AShooterCharacter::GetCrosshairSpreadMax()
{
	return CrosshairSpreadMax;
}

float AShooterCharacter::InterpCurrentFOV(float TargetFOV, float DeltaTime)
{
	return FMath::FInterpTo(CameraCurrentFOV, TargetFOV, DeltaTime, CameraZoomInterpSpeed);
}
