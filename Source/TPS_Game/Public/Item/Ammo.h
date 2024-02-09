#pragma once

#include "CoreMinimal.h"
#include "Item/BaseItem.h"
#include "Ammo.generated.h"

class UAmmoWidget;

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	EAT_9mm UMETA(DisplayName = "9mm"),
	EAT_AR UMETA(DisplayName = "Assault Rifle"),

	EAT_MAX UMETA(DisplayName = "Default")
};

UCLASS()
class TPS_GAME_API AAmmo : public ABaseItem
{
	GENERATED_BODY()
public:
	AAmmo();
	virtual void Tick(float DeltaSeconds) override;
protected:
	virtual void BeginPlay() override;
private:
	virtual void SetItemProperties(EItemState CurrentState);
	
	UFUNCTION()
	void AmmoOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Ammo, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UStaticMeshComponent> AmmoMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Ammo, meta=(AllowPrivateAccess="true"))
	EAmmoType AmmoType;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Ammo, meta=(AllowPrivateAccess="true"))
	int32 AmmoCount;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Ammo, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UTexture2D> AmmoIcon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<UAmmoWidget> AmmoWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Item Properties", meta=(AllowPrivateAccess = "true"))
	TObjectPtr<USphereComponent> AmmoPickUpCollision;
public:
	virtual void SetCustomDepthEnabled(bool bIsEnabled) override;
	FORCEINLINE EAmmoType GetAmmoType() const { return AmmoType; }
	FORCEINLINE int32 GetAmmoCount() const { return AmmoCount; }
};
