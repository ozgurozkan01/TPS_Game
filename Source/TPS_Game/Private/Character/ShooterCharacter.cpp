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

// Sets default values
AShooterCharacter::AShooterCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetRootComponent());
	CameraBoom->TargetArmLength = 300.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->SocketOffset = FVector(0.f, 50.f, 50.f);
	
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
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Set the animation BP class of character mesh
	GetMesh()->SetAnimClass(AnimationClass);
	
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

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
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
	PlayFireSoundCue();
	PlayBarrelMuzzleFlash();
	PlayGunFireMontage();
	Shoot();
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
	if(MuzzleFlash)
	{
		FTransform SocketTransform = GetSocketTransform("BarrelSocket");
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), MuzzleFlash, SocketTransform);
	}
}

void AShooterCharacter::Shoot()
{
	FTransform SocketTransform = GetSocketTransform("BarrelSocket");

	FVector Start { SocketTransform.GetLocation() };
	FQuat Rotation { SocketTransform.GetRotation() };
	FVector RotationX { Rotation.GetAxisX() };
	FVector End { Start + RotationX * 50'000.f };
	FVector SmokeEnd { End };
	
	FHitResult FireHit;
	GetWorld()->LineTraceSingleByChannel(FireHit, Start, End, ECollisionChannel::ECC_Visibility);

	if (FireHit.bBlockingHit)
	{
		SmokeEnd = {FireHit.Location};
		PlayHitParticle(FireHit.Location);
	}

	PlayBeamParticle(SocketTransform, SmokeEnd);
}

FTransform AShooterCharacter::GetSocketTransform(FName SocketName)
{
	const USkeletalMeshSocket* BarrelSocket = GetMesh()->GetSocketByName("BarrelSocket");

	if (BarrelSocket)
	{
		FTransform SocketTransform = BarrelSocket->GetSocketTransform(GetMesh());
		return SocketTransform;
	}

	return FTransform();
}

void AShooterCharacter::PlayHitParticle(FVector& HitLocation)
{
	if (HitParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle, HitLocation);
	}
}

void AShooterCharacter::PlayBeamParticle(FTransform& StartTransform, FVector& End)
{
	if (SmokeBeamParticle)
	{
		/** Spawn the Beam Particle and Store in the variable */
		TObjectPtr<UParticleSystemComponent> BeamParticleComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SmokeBeamParticle, StartTransform);
		/** This Particle has the target point and target point represents the end location. If we do not set, the end point is set FVector(0, 0, 0)
		 * To set this variable we need to store it in variable. */
		BeamParticleComponent->SetVectorParameter(FName("Target"), End);
	}
}
