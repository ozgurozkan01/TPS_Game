// Fill out your copyright notice in the Description page of Project Settings.

#include "Character/ShooterCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/AnimatorComponent.h"
#include "Components/CombatComponent.h"
#include "Components/TracerComponent.h"
#include "Components/CrosshairAnimatorComponent.h"
#include "Components/InventoryComponent.h"
#include "Components/MotionComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Item/BaseItem.h"
#include "Item/Weapon.h"

AShooterCharacter::AShooterCharacter() :
	// Camera Field Of View values to use aiming/not aiming
	CameraDefaultFOV(0.f),
	CameraZoomedFOV(40.f),
	CameraCurrentFOV(0.f),
	CameraZoomInterpSpeed(25.f),
	CameraForwardDistance(120.f),
	CameraUpDistance(30.f),
	bFireButtonPressed(false),
	bAimingButtonPressed(false)
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
	MotionComponent = CreateDefaultSubobject<UMotionComponent>(TEXT("MotionComponent"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	AnimatorComponent = CreateDefaultSubobject<UAnimatorComponent>(TEXT("AnimatorComponent"));
	InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	
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

	
	
	if (FollowCamera)
	{
		CameraDefaultFOV = GetFollowCamera()->FieldOfView;
		CameraCurrentFOV = CameraDefaultFOV;
	}

	if (EquippedWeapon)
	{
		EquippedWeapon->SetGlowMaterialEnabled(1.f);
		EquippedWeapon->SetSlotIndex(0);
	}
	
	// Set the animation BP class of character mesh
	GetMesh()->SetAnimClass(AnimationClass);
	EquipWeapon(SpawnDefaultWeapon());
	if (InventoryComponent)
	{ 
		InventoryComponent->AddElementToInventory(EquippedWeapon);
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
	
	InitializeInterpLocationContainer();
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
		EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this,  &AShooterCharacter::Movement);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this,  &AShooterCharacter::LookAround);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this,  &AShooterCharacter::Jump);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AShooterCharacter::FireButtonPressed);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AShooterCharacter::FireButtonReleased);
		EnhancedInputComponent->BindAction(ScopeAction, ETriggerEvent::Triggered, this, &AShooterCharacter::OpenScope);
		EnhancedInputComponent->BindAction(ScopeAction, ETriggerEvent::Completed, this, &AShooterCharacter::CloseScope);
		EnhancedInputComponent->BindAction(SelectAction, ETriggerEvent::Started, this, &AShooterCharacter::SelectButtonPressed);
		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AShooterCharacter::ReloadButtonPressed);
		EnhancedInputComponent->BindAction(CrouchingAction, ETriggerEvent::Triggered, this, &AShooterCharacter::CrouchingButtonPressed);
		EnhancedInputComponent->BindAction(CrouchingAction, ETriggerEvent::Completed, this, &AShooterCharacter::CrouchingButtonReleased);
		EnhancedInputComponent->BindAction(SlideAction, ETriggerEvent::Started, this,  &AShooterCharacter::SlidingButtonPressed);
		
		EnhancedInputComponent->BindAction(Key_1_Action, ETriggerEvent::Started, this, &AShooterCharacter::KeyOnePressed);
		EnhancedInputComponent->BindAction(Key_2_Action, ETriggerEvent::Started, this, &AShooterCharacter::KeyTwoPressed);
		EnhancedInputComponent->BindAction(Key_3_Action, ETriggerEvent::Started, this, &AShooterCharacter::KeyThreePressed);
		EnhancedInputComponent->BindAction(Key_4_Action, ETriggerEvent::Started, this, &AShooterCharacter::KeyFourPressed);
		EnhancedInputComponent->BindAction(Key_5_Action, ETriggerEvent::Started, this, &AShooterCharacter::KeyFivePressed);
	}
}

void AShooterCharacter::Jump()
{
	if (MotionComponent && !MotionComponent->GetIsCrouching()) { Super::Jump(); }
}

void AShooterCharacter::Movement(const FInputActionValue& Value)
{
	if (!GetCharacterMovement()->IsFalling() && MotionComponent && !GetMotionComponent()->IsSlidingOnGround())
	{
		FVector2D MovementDirection = Value.Get<FVector2D>();
		MotionComponent->Movement(MovementDirection);
	}
}

void AShooterCharacter::LookAround(const FInputActionValue& Value)
{
	FVector2D LookDirection = Value.Get<FVector2D>();

	if (MotionComponent) { MotionComponent->LookAround(LookDirection); }
}	

void AShooterCharacter::OpenScope(const FInputActionValue& Value)
{
	bAimingButtonPressed = Value.Get<bool>();

	if (bAimingButtonPressed &&
		CombatComponent &&
		CombatComponent->GetCombatState() != ECombatState::ECS_Reloading &&
		CombatComponent->GetCombatState() != ECombatState::ECS_Equipping &&
		MotionComponent)
	{
		GEngine->AddOnScreenDebugMessage(-1 , -1, FColor::Green, TEXT("OpenScope"));
		CombatComponent->StartAim();
	}
}

void AShooterCharacter::CloseScope(const FInputActionValue& Value)
{
	if (MotionComponent)
	{
		bAimingButtonPressed = false;
		CombatComponent->StopAim();
	}
}

void AShooterCharacter::SelectButtonPressed(const FInputActionValue& Value)
{
	bool bIsPressed = Value.Get<bool>();

	if (bIsPressed && TracerComponent && CombatComponent->GetCombatState() == ECombatState::ECS_Unoccupied)
	{
		TracerComponent->InterpolateItem();
	}
}

void AShooterCharacter::ReloadButtonPressed(const FInputActionValue& Value)
{
	bool bIsReloaded = Value.Get<bool>();
	if (bIsReloaded && CombatComponent)
	{
		CombatComponent->ReloadWeapon();
	}
}

void AShooterCharacter::CrouchingButtonPressed(const FInputActionValue& Value)
{
	if (!GetCharacterMovement()->IsFalling() && MotionComponent)
	{
		bool bCrouchingPressed = Value.Get<bool>();
		MotionComponent->CrouchStart(bCrouchingPressed);
	}
}

void AShooterCharacter::CrouchingButtonReleased(const FInputActionValue& Value)
{
	if (!GetCharacterMovement()->IsFalling() && MotionComponent)
	{
		bool bCrouchingReleased = Value.Get<bool>();
		MotionComponent->CrouchStop(bCrouchingReleased);
	}
}

void AShooterCharacter::FireButtonPressed(const FInputActionValue& Value)
{
	bFireButtonPressed = Value.Get<bool>();

	if (bFireButtonPressed && EquippedWeapon && TracerComponent && CombatComponent && CombatComponent->GetCombatState() == ECombatState::ECS_Unoccupied)
	{
		CombatComponent->ShootingStart();
	}
}

void AShooterCharacter::FireButtonReleased(const FInputActionValue& Value)
{
	bFireButtonPressed = false;
}

void AShooterCharacter::KeyOnePressed(const FInputActionValue& Value)
{
	if (InventoryComponent)
	{
		InventoryComponent->ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 1);
	}
}

void AShooterCharacter::KeyTwoPressed(const FInputActionValue& Value)
{
	if (InventoryComponent)
	{
		InventoryComponent->ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 2);
	}
}

void AShooterCharacter::KeyThreePressed(const FInputActionValue& Value)
{
	
	if (InventoryComponent)
	{
		InventoryComponent->ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 3);
	}
}

void AShooterCharacter::KeyFourPressed(const FInputActionValue& Value)
{
	if (InventoryComponent)
	{
		InventoryComponent->ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 4);
	}
}

void AShooterCharacter::KeyFivePressed(const FInputActionValue& Value)
{
	if (InventoryComponent)
	{
		InventoryComponent->ExchangeInventoryItems(EquippedWeapon->GetSlotIndex(), 5);
	}
}

void AShooterCharacter::SlidingButtonPressed(const FInputActionValue& Value)
{
	bool bSlidingKeyPressed = Value.Get<bool>();

	/*FString Sliding = bSlidingKeyPressed? "true" : "false";
	FString CanSliding = GetMotionComponent()->CanSlide()? "true" : "false";

	UE_LOG(LogTemp, Warning, TEXT("%s, %s"), *Sliding, *CanSliding);*/
	//UE_LOG(LogTemp, Warning, TEXT("Pressed 1"));


	/*FString MoveForward = bSlidingKeyPressed? "true" : "false";
	FString IsFall = GetMotionComponent()? "true" : "false";
	FString IsCrouch = GetMotionComponent()->CanSlide()? "true" : "false";
	FString AimPressed = !GetMotionComponent()->IsSlidingOnGround()? "true" : "false";

	UE_LOG(LogTemp, Warning, TEXT("%s, %s, %s, %s"), *MoveForward, *IsFall, *IsCrouch, *AimPressed);*/
	
	if (bSlidingKeyPressed && GetMotionComponent() && GetMotionComponent()->CanSlide() && !GetMotionComponent()->IsSlidingOnGround())
	{
		MotionComponent->Slide();
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

		if (EquippedWeapon == nullptr)
		{
			EquipItemDelegate.Broadcast(-1, WeaponToEquip->GetSlotIndex());
		}

		else
		{
			EquipItemDelegate.Broadcast(EquippedWeapon->GetSlotIndex(), WeaponToEquip->GetSlotIndex());
			}
		
		EquippedWeapon = WeaponToEquip;
		EquippedWeapon->SetItemState(EItemState::EIS_Equipped);	
	}
}

FInterpLocation AShooterCharacter::GetInterpLocation(int32 Index)
{
	if (Index < ItemInterpLocations.Num() && Index >= 0) { return ItemInterpLocations[Index]; }
	return FInterpLocation();
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

FVector AShooterCharacter::GetCameraInterpLocation()
{
	const FVector CameraWorldLocation { FollowCamera->GetComponentLocation() };
	const FVector CameraForwardVector { FollowCamera->GetForwardVector() };

	return CameraWorldLocation + CameraForwardVector * CameraForwardDistance + FVector::UpVector * CameraUpDistance;
}

float AShooterCharacter::InterpCurrentFOV(float TargetFOV, float DeltaTime)
{
	return FMath::FInterpTo(CameraCurrentFOV, TargetFOV, DeltaTime, CameraZoomInterpSpeed);
}

void AShooterCharacter::CameraInterpZoom(float DeltaTime)
{
	if (CombatComponent && CombatComponent->GetIsAiming()) { CameraCurrentFOV = InterpCurrentFOV(CameraZoomedFOV, DeltaTime); }
	else { CameraCurrentFOV = InterpCurrentFOV(CameraDefaultFOV, DeltaTime); }
	if (FollowCamera) { FollowCamera->SetFieldOfView(CameraCurrentFOV); }
}