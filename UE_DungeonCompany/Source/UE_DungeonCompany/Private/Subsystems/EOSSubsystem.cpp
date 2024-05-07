// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/EOSSubsystem.h"

#include "OnlineSubsystemUtils.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

void UEOSSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    IOnlineSubsystem* subSystem = IOnlineSubsystem::Get();

    if (!subSystem)
        return;

    UE_LOG(LogTemp, Log, TEXT("Subsystem exists"));

    SessionInterface = subSystem->GetSessionInterface();

    if (!SessionInterface.IsValid())
        return;

    UE_LOG(LogTemp, Log, TEXT("Sessioninterface exists"));

    SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UEOSSubsystem::OnCreateSessionComplete);
    SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UEOSSubsystem::OnFindSessionComplete);
    SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UEOSSubsystem::OnJoinSessionComplete);
    SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UEOSSubsystem::OnDestroySessionCompleted);
    SessionInterface->OnSessionUserInviteAcceptedDelegates.AddUObject(this, &UEOSSubsystem::OnSessionUserInviteAccepted);

    IdentityInterface = subSystem->GetIdentityInterface();

    if(!IdentityInterface.IsValid())
        return;

    UE_LOG(LogTemp, Log, TEXT("Identityinterface exists"));

    IdentityInterface->OnLoginCompleteDelegates->AddUObject(this, &UEOSSubsystem::OnLoginCompleted);
}

void UEOSSubsystem::Login()
{
    //FUniqueNetIdPtr netId = IdentityInterface->GetUniquePlayerId(0);
    //
    //if (netId != nullptr && IdentityInterface->GetLoginStatus(0) == ELoginStatus::LoggedIn)
    //    return;
    //
    //UE_LOG(LogTemp, Log, TEXT("Logging into EOS"));
    //
    //if (!IdentityInterface->AutoLogin(0))
    //    UE_LOG(LogTemp, Warning, TEXT("Failed to login"));

    FOnlineAccountCredentials credentials;
    credentials.Id = FString();
    credentials.Token = FString();
    credentials.Type = FString("accountportal");

    IdentityInterface->Login(0, credentials);

}

bool UEOSSubsystem::RegisterPlayer(APlayerController* NewPlayer)
{
    if (!NewPlayer)
        return false;

    FUniqueNetIdRepl uniqueNetIdRepl;

    ULocalPlayer* localPlayer = NewPlayer->IsLocalController()? NewPlayer->GetLocalPlayer() : nullptr;

    if (localPlayer)
        uniqueNetIdRepl = localPlayer->GetPreferredUniqueNetId();
    else
    {
        UNetConnection* remoteNetConnection = Cast<UNetConnection>(NewPlayer->Player);
        if (!IsValid(remoteNetConnection))
            return false;

        uniqueNetIdRepl = remoteNetConnection->PlayerId;
    }

    FUniqueNetIdPtr uniqueNetId = uniqueNetIdRepl.GetUniqueNetId();

    if (!uniqueNetId)
        return false;

    return SessionInterface->RegisterPlayer(NAME_GameSession, *uniqueNetId, false);
}

void UEOSSubsystem::CreateSession(FString ServerName, FString HostName)
{
    FOnlineSessionSettings sessionSettings;

    sessionSettings.bIsDedicated = false;
    sessionSettings.bAllowInvites = true;
    sessionSettings.bAllowJoinInProgress = true;
    sessionSettings.bAllowJoinViaPresence = true;
    sessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
    sessionSettings.bShouldAdvertise = true;
    sessionSettings.bUsesPresence = true;
    sessionSettings.NumPublicConnections = 4;
    sessionSettings.bUseLobbiesIfAvailable = false;
    //sessionSettings.bUseLobbiesVoiceChatIfAvailable = false;
    sessionSettings.Set(FName("SERVER_NAME_KEY"), ServerName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);
    sessionSettings.Set(FName("SERVER_HOSTNAME_KEY"), HostName, EOnlineDataAdvertisementType::ViaOnlineServiceAndPing);

    SessionInterface->CreateSession(0, NAME_GameSession, sessionSettings);
}

void UEOSSubsystem::FindSessions()
{
    UE_LOG(LogTemp, Log, TEXT("Searching for Sessions..."));
    SessionSearch = MakeShareable(new FOnlineSessionSearch());

    SessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
    SessionSearch->MaxSearchResults = 20;
    SessionSearch->QuerySettings.SearchParams.Empty();

    SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
}

void UEOSSubsystem::JoinSession(int32 Index)
{
    FOnlineSessionSearchResult result = SessionSearch->SearchResults[Index];

    //if (!result.IsValid())
    //{
    //    UE_LOG(LogTemp, Warning, TEXT("Joining session with index %d failed"), Index);
    //    return;
    //}

    UE_LOG(LogTemp, Log, TEXT("Joining session at index %d ..."), Index);
    SessionInterface->JoinSession(0, NAME_GameSession, result);
}

void UEOSSubsystem::DestroySession()
{
    if(!SessionInterface->GetNamedSession(NAME_GameSession))
        return;

    SessionInterface->DestroySession(NAME_GameSession);
}

void UEOSSubsystem::OnLoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
    if (bWasSuccessful)
        UE_LOG(LogTemp, Log, TEXT("Login completed!"))
    else
        UE_LOG(LogTemp, Warning, TEXT("EOS login failed."));
}

void UEOSSubsystem::OnCreateSessionComplete(FName SessionName, bool Succeeded)
{
    UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete Succeeded: %d"), Succeeded);

    if (!Succeeded)
        return;

    GetWorld()->ServerTravel("/Game/_DungeonCompanyContent/Maps/TestMap?listen");
}

void UEOSSubsystem::OnFindSessionComplete(bool Succeeded)
{
    UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete Succeeded: %d"), Succeeded);
    UE_LOG(LogTemp, Warning, TEXT("FoundSession Num: %d"), SessionSearch->SearchResults.Num());

    if (!Succeeded)
        return;

    TArray<FServerInfo> infos;
    int i = 0;

    for (FOnlineSessionSearchResult& SR : SessionSearch->SearchResults)
    {
        //if (!SR.IsValid())
        //    continue;

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

void UEOSSubsystem::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
    if(Result == EOnJoinSessionCompleteResult::Success)
        UE_LOG(LogTemp, Log, TEXT("Successfully joined Session!"))
    else 
        UE_LOG(LogTemp, Warning, TEXT("Joining Session failed!"));

    APlayerController* controller = GetWorld()->GetFirstPlayerController();

    if (!controller)
        return;

    FString joinAdress = "";
    SessionInterface->GetResolvedConnectString(SessionName, joinAdress);

    joinAdress = joinAdress.Replace(*FString(":0"), *FString(":7777"), ESearchCase::IgnoreCase);

    if (joinAdress == "")
        return;

    controller->ClientTravel(joinAdress, ETravelType::TRAVEL_Absolute);
}

void UEOSSubsystem::OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful)
{

}

void UEOSSubsystem::OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult)
{
    SessionInterface->JoinSession(0, NAME_GameSession, InviteResult);
}