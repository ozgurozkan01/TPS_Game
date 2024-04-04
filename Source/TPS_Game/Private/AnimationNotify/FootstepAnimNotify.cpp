// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimationNotify/FootstepAnimNotify.h"

void UFootstepAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(250,
								10,
								FColor::Red,
								FString::Printf(TEXT("Bone Name : %s"),
								*BoneName.ToString()));
	}
}
