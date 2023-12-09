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
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(30.f),
	CameraCurrentFOV(0.f),
	CameraZoomInterpSpeed(25.f),
	bAiming(false)
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
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

	AddControllerYawInput(LookDirection.X);
	AddControllerPitchInput(LookDirection.Y);
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

FTransform AShooterCharacter::GetGunBarrelSocketTransform(FName GunBarrelSocket)
{
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName(GunBarrelSocket);

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

void AShooterCharacter::PlayRightBarrelMuzzleFlash()
{
	if(MuzzleFlash)
	{
		FTransform RightBarrelSocketTransform = GetGunBarrelSocketTransform("RightWeaponBarrelSocket");
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, RightBarrelSocketTransform);
	}
}

void AShooterCharacter::PlayLeftBarrelMuzzleFlash()
{
	if (MuzzleFlash)
	{
		FTransform LeftBarrelSocketTransform = GetGunBarrelSocketTransform("LeftWeaponBarrelSocket");
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, LeftBarrelSocketTransform);
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
	FTransform RightBarrelSocketTransform = GetGunBarrelSocketTransform("RightWeaponBarrelSocket");
	FTransform LeftBarrelSocketTransform = GetGunBarrelSocketTransform("LeftWeaponBarrelSocket");
	/*
	DrawDebugSphere(GetWorld(), RightBarrelSocketTransform.GetLocation(), 25, 15, FColor::Red, true);
	DrawDebugSphere(GetWorld(), LeftBarrelSocketTransform.GetLocation(), 25, 15, FColor::Green, true);
	*/

	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;

	if (IsConvertedScreenToWorld(CrosshairWorldPosition, CrosshairWorldDirection))
	{
		FVector BeamEndPoint { FVector::ZeroVector };
		LineTraceFromTheScreen(CrosshairWorldPosition, CrosshairWorldDirection, BeamEndPoint);
		LineTraceFromTheGunBarrel(RightBarrelSocketTransform.GetLocation(), BeamEndPoint);
		LineTraceFromTheGunBarrel(LeftBarrelSocketTransform.GetLocation(), BeamEndPoint);

		
		PlayHitParticle(BeamEndPoint);
		PlayBeamParticle(LeftBarrelSocketTransform, BeamEndPoint);

		//DrawDebugSphere(GetWorld(), BeamEndPoint, 25, 15, FColor::Blue, true);
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

float AShooterCharacter::InterpCurrentFOV(float TargetFOV, float DeltaTime)
{
	return FMath::FInterpTo(CameraCurrentFOV, TargetFOV, DeltaTime, CameraZoomInterpSpeed);
}
