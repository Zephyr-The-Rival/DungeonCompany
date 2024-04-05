// Fill out your copyright notice in the Description page of Project Settings.



#include "UE_DungeonCompany/Public/DungeonCompanyGameInstance.h"
#include "Engine/World.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"
#include "Online/OnlineSessionNames.h"

UDungeonCompanyGameInstance::UDungeonCompanyGameInstance()
{

}
void UDungeonCompanyGameInstance::Init()
{
	if (IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get())
	{
		sessionInterface = SubSystem->GetSessionInterface();
		if (sessionInterface.IsValid())
		{
			//Bind Delegates here
			sessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UDungeonCompanyGameInstance::OnCreateSessionComplete);
			sessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UDungeonCompanyGameInstance::OnFindSessionComplete);
		}
	}
}

void UDungeonCompanyGameInstance::OnCreateSessionComplete(FName serverName, bool succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete Succeeded: %d"), succeeded);
	if (succeeded)
	{
		GetWorld()->ServerTravel("/Game/_DungeonCompanyContent/Maps/TestMap?listen");
	}
}

void UDungeonCompanyGameInstance::OnFindSessionComplete(bool succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnFindSessionComplete Succeeded: %d"), succeeded);

	if (succeeded)
	{
		TArray<FOnlineSessionSearchResult> searchResults = sessionSearch->SearchResults;		
		UE_LOG(LogTemp, Warning, TEXT("FoundServers: %d"), searchResults.Num());
	}
}



void UDungeonCompanyGameInstance::CreateServer()
{
	UE_LOG(LogTemp, Warning, TEXT("CreatingServer..."));
	FOnlineSessionSettings sessionSettings;
	sessionSettings.bAllowJoinInProgress =true;
	sessionSettings.bIsDedicated = false;
	sessionSettings.bIsLANMatch = true;
	sessionSettings.bShouldAdvertise = true;
	sessionSettings.bUsesPresence = true;
	sessionSettings.NumPublicConnections = 5;
	sessionInterface->CreateSession(0, FName("MySession"), sessionSettings);
}

void UDungeonCompanyGameInstance::JoinServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Searching for Sessions..."));
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	sessionSearch->bIsLanQuery = true;
	sessionSearch->MaxSearchResults = 10000;//big number because of other steam users with the same appId
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}