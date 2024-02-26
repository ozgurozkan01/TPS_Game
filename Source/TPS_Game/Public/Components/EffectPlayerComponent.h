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
};
