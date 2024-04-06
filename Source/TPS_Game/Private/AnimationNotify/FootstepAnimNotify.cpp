// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimationNotify/FootstepAnimNotify.h"

#include "Kismet/GameplayStatics.h"

void UFootstepAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                 const FAnimNotifyEventReference& EventReference)
{
	//ConstructorHelpers::FObjectFinder<USoundBase> FoundSound(TEXT("OBJECT PATH"));
	//FootstepCue = FoundSound.Object;
	
	if (FootstepCue && MeshComp)
	{
		FVector SocketLocation = MeshComp->GetSocketLocation(SocketNameOnBone);
		// Play Footstep Sound
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), FootstepCue, SocketLocation);
	}
}