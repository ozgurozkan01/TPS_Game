// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

class UCurveVector;
class USoundCue;
class UInformationPopUp;
class USphereComponent;
class UWidgetComponent;
class UBoxComponent;
class UCurveFloat;
class AShooterCharacter;

UENUM(BlueprintType)
enum class EItemState : uint8
{
	EIS_Equipped UMETA(DisplayName = "Equipped"),
	EIS_Pickup UMETA(DisplayName = "Pickup"),
	EIS_EquipInterping UMETA(DisplayName = "EquipInterping"),
	EIS_PickedUp UMETA(DisplayName = "PickedUp"),
	EIS_Falling UMETA(DisplayName = "Falling"),

	EIS_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Uncommon UMETA(DisplayName = "Uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_MAX UMETA(DisplayName = "DefaultMAX"),
};

UENUM(BlueprintType)
enum class EItemType
{
	EIT_Ammo UMETA(DisplayName = "Ammo"),
	EIT_Weapon UMETA(DisplayName = "Weapon"),
	
	EIT_Default UMETA(DisplayName = "Default")
};

UCLASS()
class TPS_GAME_API  ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseItem();
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void Tick(float DeltaTime) override;
	
protected:
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	virtual void BeginPlay() override;
	/** Called when is interping finished */
	void FinishInterping();
	void StartGlowPulseTimer();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> PickupSoundCue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> EquipSoundCue;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ItemMesh;	
	/** Line Trace collides with it to show the HUD which has information about item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> CollisionBox;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> InformationWidgetComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> TraceCheckSphere;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	FString ItemName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	EItemType ItemType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	EItemState ItemState;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;
	/** Material instance that we can change at runtime */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInstanceDynamic> DynamicMaterialInstance;
	/** Material instance  */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UMaterialInstance> MaterialInstance;
private:
	void SinusodialMovement();
	void Rotate();
	/** Handle interpolation and curve motion when in the EquipInterping state */
	void ItemInterp(float DeltaTime);
	/** Curve Asset to update Z Value */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> ItemZCurve;
	/** Curve Asset to update Scale Value */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCurveFloat> ItemScaleCurve;
	/** Starting ocation of item when interping begins */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	FVector ItemStartInterpLocation; 
	/** Target interp location in front of the camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	FVector CameraTargetLocation;
	/** Interping controller value */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	bool bIsInterping;
	/** Activates when interping begins */
	FTimerHandle ItemInterpTimer;
	/** Duration of curve and timer */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	float ZCurveTime;
	float ItemInterpingX;
	float ItemInterpingY;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<AShooterCharacter> ShooterRef;	
	float SinusodialSpeed;
	float AmplitudeMultiplier;
	float YawRotationRate;
	float ItemInitialYawOffset;
	bool bCanIdleMove;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> ExchangeSoundCue;

	FTimerHandle ExchangeSoundTimerHandle;
	float ExchangeDelayTime;
	
	int32 InterpLocationIndex;
	
	/** Material Glowing Pulse Variables */
	FTimerHandle PulseTimerHandle;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	float PulseCurveTime;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	float GlowAmount;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	float FresnelExponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	float FresnelReflectFraction;
	/** Curve to update dynamic material scalar parameters */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UCurveVector> GlowPulseCurve;

	void ResetGlowPulseTimer();
	void UpdateGlowPulseScalars();
	
public:

	virtual void SetCustomDepthEnabled(bool bIsEnabled);
	virtual void SetGlowMaterialEnabled(float Value);
	FVector GetInterpLocCorrespondItemType();
	
	/** Start curve from the shooter class */
	void StartItemCurve(TObjectPtr<AShooterCharacter> Shooter);
	void PlayPickupSoundCue();
	void PlayEquipSoundCue();
	void PlayExchangeSound(bool bIsUsedTimer);
	void StartExchangeSoundTimer();
	
	// Setter
	void SetItemCollisions(bool bCanCollide);
	virtual void SetItemProperties(EItemState CurrentState);
	void SetItemState(EItemState CurrentState);
	FORCEINLINE void SetIdleMovement(bool bCanMove) { bCanIdleMove = bCanMove; };

	// Getter
	FORCEINLINE TObjectPtr<UWidgetComponent> GetInformationWidgetComponent() const { return InformationWidgetComponent; };
	FORCEINLINE TObjectPtr<USkeletalMeshComponent> GetItemSkeletalMesh() const { return ItemMesh; };
	FORCEINLINE TObjectPtr<USkeletalMeshComponent> GetItemMesh() const { return ItemMesh; }
	FORCEINLINE FString GetItemName() const { return ItemName; };
	FORCEINLINE EItemRarity GetItemRarity() const { return ItemRarity; }
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
};
