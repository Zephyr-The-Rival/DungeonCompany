// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/DungeonCompanyPlayerController.h"

#include "UObject/ConstructorHelpers.h"
#include "Net/VoiceConfig.h"

ADungeonCompanyPlayerController::ADungeonCompanyPlayerController()
{
	static ConstructorHelpers::FObjectFinder<USoundAttenuation> voiceSA(TEXT("/Game/_DungeonCompanyContent/Audio/Player/VoiceSA.VoiceSA"));
	
	VoiceSA = voiceSA.Object;
}

void ADungeonCompanyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly mode;
	this->SetInputMode(mode);

	CreateVOIPTalker();

}

void ADungeonCompanyPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (IsValid(VOIPTalker))
		VOIPTalker->Settings.ComponentToAttachTo = InPawn->GetRootComponent();
}

void ADungeonCompanyPlayerController::CreateVOIPTalker()
{
	APlayerState* playerState = GetPlayerState<APlayerState>();

	if (!IsValid(VOIPTalker) && playerState)
		VOIPTalker = UVOIPTalker::CreateTalkerForPlayer(playerState);

	if (!IsValid(VOIPTalker))
		return;

	if (APawn* pawn = GetPawn())
		VOIPTalker->Settings.ComponentToAttachTo = pawn->GetRootComponent();

	VOIPTalker->Settings.AttenuationSettings = VoiceSA;

	UVOIPStatics::SetMicThreshold(-1.0);

	ToggleSpeaking(true);
}