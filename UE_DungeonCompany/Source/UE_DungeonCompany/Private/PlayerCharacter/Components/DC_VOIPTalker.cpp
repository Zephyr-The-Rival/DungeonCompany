// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/Components/DC_VOIPTalker.h"

#include "Subsystems/VoiceChatSubsystem.h"

void UDC_VOIPTalker::OnTalkingBegin(UAudioComponent* AudioComponent)
{
	Super::OnTalkingBegin(AudioComponent);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("TALKING"));
	
	if (!IsValid(AudioComponent) || TalkerAudioComponent == AudioComponent)
		return;

	TalkerAudioComponent = AudioComponent;

	APawn* ownerPawn = GetOwner<APawn>();
	if (!ownerPawn)
		return;

	TalkerAudioComponent->SoundClassOverride = GetWorld()->GetGameInstance()->GetSubsystem<
		UVoiceChatSubsystem>()->GetVoiceClassForPlayer(ownerPawn->GetPlayerState());
}
