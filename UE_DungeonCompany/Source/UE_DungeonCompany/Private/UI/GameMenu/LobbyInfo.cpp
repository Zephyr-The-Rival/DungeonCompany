// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameMenu/LobbyInfo.h"

#include "EngineUtils.h"
#include "Blueprint/WidgetTree.h"
#include "Components/VerticalBox.h"
#include "GameFramework/PlayerState.h"
#include "UI/GameMenu/PlayerOptions.h"
#include "UObject/ConstructorHelpers.h"

ULobbyInfo::ULobbyInfo()
{
 	//static ConstructorHelpers::FClassFinder<UPlayerOptions> PlayerOptionsClassFinder(
	//	TEXT("/Game/_DungeonCompanyContent/Code/UI/GameMenu/WBP_PlayerOptions"));
	//
	//PlayerOptionsClass = PlayerOptionsClassFinder.Class;
}

void ULobbyInfo::NativeConstruct()
{
	Super::NativeConstruct();

	for (TActorIterator<APlayerState> It(GetWorld()); It; ++It)
	{
		APlayerState* playerState = *It;

		if (APlayerController* playerController = playerState->GetPlayerController(); playerController &&
			playerController->IsLocalController())
			continue;

		AddPlayerOptionsForState(playerState);
	}
}

void ULobbyInfo::AddPlayerOptionsForState(APlayerState* PlayerState)
{
	UPlayerOptions* playerOptions = WidgetTree->ConstructWidget<UPlayerOptions>(PlayerOptionsClass);
	playerOptions->SetControllingPlayer(PlayerState);
	LobbyOptionsVerticalBox->AddChild(playerOptions);
	PlayerOptions.Add(playerOptions);
}
