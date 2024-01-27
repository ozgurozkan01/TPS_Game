// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

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
class TPS_GAME_API ABaseItem : public AActor
{
	GENERATED_BODY()
	
public:	
	ABaseItem();
	virtual void Tick(float DeltaTime) override;
	
protected:
	virtual void BeginPlay() override;
	/** Called when is interping finished */
	void FinishInterping();

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
private:

	virtual void InitializeCustomDepth();
	void SinusodialMovement();
	void Rotate();
	/** Handle interpolation and curve motion when in the EquipInterping state */
	void ItemInterp(float DeltaTime);
	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	EItemState ItemState;
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
	TObjectPtr<USoundBase> PickupSoundCue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USoundBase> EquipSoundCue;

	int32 InterpLocationIndex;
	
public:

	virtual void SetCustomDepthEnabled(bool bIsEnabled);
	FVector GetInterpLocCorrespondItemType();
	
	/** Start curve from the shooter class */
	void StartItemCurve(TObjectPtr<AShooterCharacter> Shooter);
	void PlayPickupSoundCue();
	void PlayEquipSoundCue();
	
	// Setter
	void SetItemCollisions(bool bCanCollide);
	virtual void SetItemProperties(EItemState CurrentState);
	void SetItemState(EItemState CurrentState);
	FORCEINLINE void SetIdleMovement(bool bCanMove) { bCanIdleMove = bCanMove; };

	// Getter
	FORCEINLINE TObjectPtr<UWidgetComponent> GetInformationWidgetComponent() const { return InformationWidgetComponent; };
	FORCEINLINE TObjectPtr<USkeletalMeshComponent> GetItemSkeletalMesh() const { return ItemMesh; };
	FORCEINLINE EItemState GetItemState() const { return ItemState; }
	FORCEINLINE TObjectPtr<USkeletalMeshComponent> GetItemMesh() const { return ItemMesh; }
	FORCEINLINE FString GetItemName() const { return ItemName; };
	uint8 GetActivateStarNumber();
		
};
