// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseItem.generated.h"

class UInformationPopUp;
class USphereComponent;
class UWidgetComponent;
class UBoxComponent;

UENUM()
enum class EItemRarity : uint8
{
	EIR_Damaged UMETA(DisplayName = "Damaged"),
	EIR_Common UMETA(DisplayName = "Common"),
	EIR_Uncommon UMETA(DisplayName = "Uncommon"),
	EIR_Rare UMETA(DisplayName = "Rare"),
	EIR_Legendary UMETA(DisplayName = "Legendary"),

	EIR_MAX UMETA(DisplayName = "DefaultMAX"),
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

private:
	void SinusodialMovement();
	void Rotate();

	UFUNCTION()
	void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> ItemMesh;	

	/** Line Trace collides with it to show the HUD which has information about item */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UBoxComponent> CollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UWidgetComponent> InformationWidgetComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UInformationPopUp> InformationWidgetObject;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> TraceCheckSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	FString ItemName;	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	EItemRarity ItemRarity;

	TArray<bool> ActiveStars;
	
	float SinusodialSpeed;
	float AmplitudeMultiplier;
	float YawRotationRate;

	bool bCanIdleMove;
public:

	
	// Setter
	void SetActiveStarts();
	void SetItemCollisions(bool bCanCollide);
	FORCEINLINE void SetIdleMovement(bool bCanMove) { bCanIdleMove = bCanMove; };
	
	// Getter
	FORCEINLINE TObjectPtr<UWidgetComponent> GetInformationWidgetComponent() const { return InformationWidgetComponent; };
	FORCEINLINE TObjectPtr<UInformationPopUp> GetInformationWidgetObject() const { return InformationWidgetObject; };
	FORCEINLINE TObjectPtr<USkeletalMeshComponent> GetItemSkeletalMesh() const { return ItemMesh; };

	uint8 GetActivateStarNumber();
		
};
