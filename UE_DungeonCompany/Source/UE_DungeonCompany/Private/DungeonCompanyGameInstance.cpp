// Fill out your copyright notice in the Description page of Project Settings.



#include "UE_DungeonCompany/Public/DungeonCompanyGameInstance.h"

#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
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
			sessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UDungeonCompanyGameInstance::OnJoinSessionComplete);
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



		for(FOnlineSessionSearchResult SR : searchResults)
		{
			if (SR.IsValid()) 
			{
				FServerInfo info;
				info.ServerName = "Test Name";
				info.MaxPlayers = SR.Session.SessionSettings.NumPublicConnections;
				info.currentPlayers = info.MaxPlayers-SR.Session.NumOpenPublicConnections;

				ServerListDel.Broadcast(info);
			}
		}


		//UE_LOG(LogTemp, Warning, TEXT("FoundServers: %d"), searchResults.Num());
		//if (searchResults.Num() != 0)
		//{
		//	UE_LOG(LogTemp, Warning, TEXT("Joining Server : %s"), *searchResults[0].Session.OwningUserName);
		//	sessionInterface->JoinSession(0, "MySession", searchResults[0]);
		//}
		
	}
}

void UDungeonCompanyGameInstance::OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result)
{
	UE_LOG(LogTemp, Warning, TEXT("OnJoinSessionComplete"));
	if (APlayerController* PController = UGameplayStatics::GetPlayerController(GetWorld(), 0))
	{
		FString JoinAdress="";
		sessionInterface->GetResolvedConnectString(sessionName, JoinAdress);

		FString a = ":0";
		FString b = ":7777";
		JoinAdress=JoinAdress.Replace(*a,*b, ESearchCase::IgnoreCase);
		if (JoinAdress != "")
		{
			UE_LOG(LogTemp, Warning, TEXT("Traveling to: %s"), *JoinAdress);
			PController->ClientTravel(JoinAdress, ETravelType::TRAVEL_Absolute);
		}
		
	}
}



void UDungeonCompanyGameInstance::CreateServer()
{
	UE_LOG(LogTemp, Warning, TEXT("CreatingServer..."));
	FOnlineSessionSettings sessionSettings;
	sessionSettings.bAllowJoinInProgress =true;
	sessionSettings.bIsDedicated = false;
	sessionSettings.bIsLANMatch = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	sessionSettings.bShouldAdvertise = true;
	sessionSettings.bUsesPresence = true;
	sessionSettings.NumPublicConnections = 5;
	sessionSettings.bUseLobbiesIfAvailable = true;
	sessionInterface->CreateSession(0, FName("MySession"), sessionSettings);
}

void UDungeonCompanyGameInstance::JoinServer()
{
	UE_LOG(LogTemp, Warning, TEXT("Searching for Sessions..."));
	sessionSearch = MakeShareable(new FOnlineSessionSearch());

	sessionSearch->bIsLanQuery = (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL");
	sessionSearch->MaxSearchResults = 10000;//big number because of other steam users with the same appId
	sessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);

	sessionInterface->FindSessions(0, sessionSearch.ToSharedRef());
}