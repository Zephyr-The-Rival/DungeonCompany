// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotifies/AnimNotify_PlayRandomSound.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "Perception/AISense_Hearing.h"

void UAnimNotify_PlayRandomSound::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (Sounds.Num() <= 0)
		return;

	if (InactiveSounds.Num() <= 0 || !Sound)
	{
		int soundsNum = Sounds.Num();

		InactiveSounds.Empty(soundsNum);

		for (int i = 0; i < soundsNum; ++i)
		{
			InactiveSounds.Add(Sounds[i]);
		}
	}

	int soundIndex = FMath::RandRange(0, InactiveSounds.Num() - 1);

	if(InactiveSounds.Num() != Sounds.Num())
		InactiveSounds.Add(Sound);

	Sound = InactiveSounds[soundIndex];

	InactiveSounds.RemoveAt(soundIndex);

	Super::Notify(MeshComp, Animation, EventReference);

	AActor* owner = MeshComp->GetOwner();

	if(!MeshComp->GetOwner()->HasAuthority())
		return;

	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(owner);

	if(!playerCharacter)
		return;

	UAISense_Hearing::ReportNoiseEvent(GetWorld(), playerCharacter->GetActorLocation(), VolumeMultiplier, playerCharacter);
	
}
