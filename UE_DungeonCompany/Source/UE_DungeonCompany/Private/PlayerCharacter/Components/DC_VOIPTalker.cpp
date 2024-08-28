// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/Components/DC_VOIPTalker.h"

#include "Subsystems/VoiceChatSubsystem.h"

void UDC_VOIPTalker::OnTalkingBegin(UAudioComponent* AudioComponent)
{
	Super::OnTalkingBegin(AudioComponent);
	
	if (!IsValid(AudioComponent))
		return;

	TalkerAudioComponent = AudioComponent;
	
	APawn* ownerPawn = GetOwner<APawn>();
	if (!ownerPawn)
		return;
	
	GetWorld()->GetGameInstance()->GetSubsystem<UVoiceChatSubsystem>()->RegisterAudioComponentForPlayerState(ownerPawn->GetPlayerState(), TalkerAudioComponent);
}
