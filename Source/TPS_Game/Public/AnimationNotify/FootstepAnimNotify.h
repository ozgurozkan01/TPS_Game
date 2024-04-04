// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "FootstepAnimNotify.generated.h"

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
	UPROPERTY(EditDefaultsOnly, Category="Notify Properties", meta=(AllowPrivateAccess="true"))
	FName BoneName;
};
