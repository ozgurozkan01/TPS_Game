// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FootstepAnimNotify.generated.h"

class USoundCue;
/**
 * 
 */
UCLASS()
class TPS_GAME_API UFootstepAnimNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
private:
	UPROPERTY(EditAnywhere, Category="Notify Properties", meta=(AllowPrivateAccess="true"))
	FName SocketNameOnBone;
	UPROPERTY(EditAnywhere, Category="Notify Properties", meta=(AllowPrivateAccess="true"))
	USoundBase* FootstepCue;
};
