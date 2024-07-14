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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> EquipWeaponMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UAnimMontage> SlidingMontage;
	UPROPERTY(EditDefaultsOnly, Category=Animation)
	TObjectPtr<UShooterAnimInstance> ShooterAnimInstance;
	UPROPERTY()
	TObjectPtr<AShooterCharacter> OwnerRef;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=State, meta=(AllowPrivateAccess = "true"))
	FTransform MagazineTransform;
public:
	void PlayGunFireMontage();
	void PlayReloadWeaponMontage();
	void PlayEquipWeaponMontage();
	void PlaySlidingMontage();
	
	UFUNCTION(BlueprintCallable)
	void FinishEquipping();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	UFUNCTION(BlueprintCallable)
	void GrabMagazine();
	UFUNCTION(BlueprintCallable)
	void ReplaceMagazine();
	UFUNCTION(BlueprintCallable)
	void FinishSliding();
};
