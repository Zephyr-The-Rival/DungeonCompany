// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/DungeonCompanyPlayerController.h"
#include "DungeonCompanyStatics.h"

#include "UObject/ConstructorHelpers.h"
#include "Net/VoiceConfig.h"

ADungeonCompanyPlayerController::ADungeonCompanyPlayerController()
{

}

void ADungeonCompanyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly mode;
	this->SetInputMode(mode);

	//SetupVOIPTalker();

}

void ADungeonCompanyPlayerController::SetupVOIPTalker()
{
	APlayerState* playerState = GetPlayerState<APlayerState>();

	if (!playerState)
	{
		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ADungeonCompanyPlayerController::SetupVOIPTalker, 0.1f);
		return;
	}

	VOIPTalker->RegisterWithPlayerState(playerState);

	VOIPTalker->Settings.AttenuationSettings = VoiceSA;

	UVOIPStatics::SetMicThreshold(-1.0);
	VOIPTalker->Settings.ComponentToAttachTo = TalkerAttachedTo;

	if (!IsLocalController())
		return;

	GetWorld()->Exec(GetWorld(), TEXT("OSS.VoiceLoopback 1"));
	ToggleSpeaking(true);

}

void ADungeonCompanyPlayerController::AttachVOIPTalkerTo(USceneComponent* AttachTo)
{
	TalkerAttachedTo = AttachTo;

	if(IsValid(VOIPTalker))
		VOIPTalker->Settings.ComponentToAttachTo = AttachTo;
}
