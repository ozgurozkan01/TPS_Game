#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EffectPlayerComponent.generated.h"

class UAnimMontage;
class UParticleSystem;
class AShooterCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TPS_GAME_API UEffectPlayerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UEffectPlayerComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY()
	TObjectPtr<AShooterCharacter> OwnerRef;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	USoundBase* FireSoundCue;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UParticleSystem> MuzzleFlash;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UParticleSystem> HitParticle;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Combat, meta=(AllowPrivateAccess="true"))
	TObjectPtr<UParticleSystem> SmokeBeamParticle;

public:
	void PlayHitParticle(const FVector& HitLocation);
	void PlayBeamParticle(const FTransform& Start, const FVector& End);
	void PlayFireSoundCue();
	void PlayBarrelMuzzleFlash();		
};
