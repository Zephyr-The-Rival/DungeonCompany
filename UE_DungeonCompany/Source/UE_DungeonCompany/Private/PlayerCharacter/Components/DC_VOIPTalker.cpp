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

void UDC_VOIPTalker::BeginPlay()
{
	Super::BeginPlay();

	GEngine->OnNetworkFailure().AddUObject(this, &UDC_VOIPTalker::HandleNetworkFailure);
}

void UDC_VOIPTalker::HandleNetworkFailure(UWorld* World, UNetDriver* NetDriver, ENetworkFailure::Type FailureType,
                                          const FString& ErrorString)
{
	if(GetOwner()->HasAuthority())
		return;
	
	Deactivate();
	DestroyComponent();
}
