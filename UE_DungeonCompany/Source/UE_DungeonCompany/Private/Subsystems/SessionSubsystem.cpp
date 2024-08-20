// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/SessionSubsystem.h"

#include "DC_Statics.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Blueprint/UserWidget.h"
#include "Online/OnlineSessionNames.h"

USessionSubsystem::USessionSubsystem()
{
	this->LoadingScreen = ConstructorHelpers::FClassFinder<UUserWidget>(TEXT("/Game/_DungeonCompanyContent/Code/UI/JoiningScreen")).Class;
}

void USessionSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get();

	if (!SubSystem)
		return;

	SessionInterface = SubSystem->GetSessionInterface();

	if (!SessionInterface.IsValid())
		return;

	SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &USessionSubsystem::OnCreateSessionComplete);
	SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &USessionSubsystem::OnFindSessionComplete);
	SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &USessionSubsystem::OnJoinSessionComplete);
	SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &USessionSubsystem::OnSessionUserInviteAccepted);

}

void USessionSubsystem::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete Succeeded: %d"), Succeeded);

	if (!Succeeded)
		return;
	
	GetWorld()->ServerTravel("/Game/_DungeonCompanyContent/Maps/MainDungeon?listen");

}

void USessionSubsystem::OnFindSessionComplete(bool Succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete Succeeded: %d"), Succeeded);

	if (!Succeeded)
		return;
	
	TArray<FServerInfo> infos;
	int i = 0;

	for (FOnlineSessionSearchResult SR : SessionSearch->SearchResults)
	{
		if(!SR.IsValid())
			continue;

		if(SR.Session.SessionSettings.NumPublicConnections==0)
			continue;
		
		FServerInfo info;
		FString serverName = "Empty server same";
		FString hostName = "Empty host name";

		SR.Session.SessionSettings.Get(FName("SERVER_NAME_KEY"), serverName);
		SR.Session.SessionSettings.Get(FName("SERVER_HOSTNAME_KEY"), hostName);

		info.ServerName = serverName;
		info.MaxPlayers = SR.Session.SessionSettings.NumPublicConnections;
		info.CurrentPlayers = info.MaxPlayers - SR.Session.NumOpenPublicConnections;
		info.SetPlayerCount();
		info.ArrayIndex = i;
		++i;

		infos.Add(info);
		
	}

	SearchComplete.Broadcast(infos);
}

void USessionSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete"));

	APlayerController* Controller = GetWorld()->GetFirstPlayerController();

	if(!Controller)
		return;

	FString JoinAdress = "";
	SessionInterface->GetResolvedConnectString(SessionName, JoinAdress);

	JoinAdress = JoinAdress.Replace(*FString(":0"), *FString(":7777"), ESearchCase::IgnoreCase);

	if (JoinAdress == "")
		return;

	UE_LOG(LogTemp, Warning, TEXT("Traveling to: %s"), *JoinAdress);
	Controller->ClientTravel(JoinAdress, ETravelType::TRAVEL_Absolute);

}

void USessionSubsystem::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
	
	if(IsValid(LoadingScreen))
		CreateWidget<UUserWidget>(GetWorld(),LoadingScreen)->AddToViewport(1);
	
	SessionInterface->JoinSession(0, NAME_GameSession, InviteResult);

}


void USessionSubsystem::CreateServer(FString ServerName, FString HostName, bool bIsPrivate)
{
	UE_LOG(LogTemp, Warning, TEXT("CreatingServer..."));
	FOnlineSessionSettings sessionSettings;

	sessionSettings.bAllowJoinInProgress = true;
	sessionSettings.bIsDedicated = false;
	sessionSettings.bIsLANMatch = false;
	sessionSettings.bShouldAdvertise = true;
	sessionSettings.bAllowInvites=true;
	sessionSettings.bAllowJoinViaPresence=true;
	sessionSettings.bUsesPresence = true;

	if(bIsPrivate)
	{
		sessionSettings.NumPublicConnections = 0;
		sessionSettings.NumPrivateConnections =4;
	}
	else
	{
		sessionSettings.NumPublicConnections = 4;
		sessionSettings.NumPrivateConnections =0;
	}
	
	sessionSettings.bUseLobbiesIfAvailable = true;
	sessionSettings.Set(FName("SERVER_NAME_KEY"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
	sessionSettings.Set(FName("SERVER_HOSTNAME_KEY"), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

	SessionInterface->CreateSession(0, NAME_GameSession, sessionSettings);

}

void USessionSubsystem::FindServers()
{
	UE_LOG(LogTemp, Warning, TEXT("Searching for Sessions..."));
	SessionSearch = MakeShareable(new FOnlineSessionSearch());

	SessionSearch->bIsLanQuery = false;
	SessionSearch->MaxSearchResults = 10000;//big number because of other steam users with the same appId
	SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
	
	SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());

}

void USessionSubsystem::JoinServer(int32 Index)
{
	FOnlineSessionSearchResult result = SessionSearch->SearchResults[Index];

	if (!result.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("Joining session with index %d failed"), Index);
		return;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("Joining session at index %d ..."), Index);
	SessionInterface->JoinSession(0, NAME_GameSession, result);
	
}

void USessionSubsystem::DestroyCurrentSession()
{
	if(!SessionInterface->GetNamedSession(NAME_GameSession))
		return;

	SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &USessionSubsystem::OnSessionLeft);
	SessionInterface->DestroySession(NAME_GameSession);

}

void USessionSubsystem::OnSessionLeft(FName SessionName, bool bWasSuccessful)
{
	if(!bWasSuccessful)
		return;
	
	GetWorld()->GetFirstPlayerController()->ClientTravel("/Game/_DungeonCompanyContent/Maps/MainMenu",TRAVEL_Absolute);
}

