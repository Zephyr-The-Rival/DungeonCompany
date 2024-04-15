// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/DungeonCompanyPlayerController.h"
#include "DungeonCompanyStatics.h"

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

}

void ADungeonCompanyPlayerController::CreateVOIPTalker()
{
	APlayerState* playerState = GetPlayerState<APlayerState>();

	if (!playerState)
	{
		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ADungeonCompanyPlayerController::CreateVOIPTalker, 0.1f);
		return;
	}

	VOIPTalker = UVOIPTalker::CreateTalkerForPlayer(playerState);

	SetupVOIPTalker();

}

void ADungeonCompanyPlayerController::SetupVOIPTalker()
{
	APawn* pawn = GetPawn();

	if (!IsValid(VOIPTalker) || !pawn)
	{
		FTimerHandle timerHandle;
		GetWorld()->GetTimerManager().SetTimer(timerHandle, this, &ADungeonCompanyPlayerController::SetupVOIPTalker, 0.1f);
		return;
	}

	VOIPTalker->Settings.ComponentToAttachTo = pawn->GetRootComponent();
	VOIPTalker->Settings.AttenuationSettings = VoiceSA;

	UVOIPStatics::SetMicThreshold(-1.0);

	ToggleSpeaking(true);

}
