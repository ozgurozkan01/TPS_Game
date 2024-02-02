#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimatorComponent.generated.h"

class UAnimMontage;
class UShooterAnimInstance;
class AShooterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_GAME_API UAnimatorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAnimatorComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> GunFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> ReloadMontage;
	UPROPERTY(EditDefaultsOnly, Category=Animation)
	TObjectPtr<UShooterAnimInstance> ShooterAnimInstance;
	UPROPERTY()
	TObjectPtr<AShooterCharacter> OwnerRef;
	
public:
	void PlayGunFireMontage();
	void PlayReloadWeaponMontage();
};
