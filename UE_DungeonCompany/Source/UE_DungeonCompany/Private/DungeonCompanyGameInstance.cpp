// Fill out your copyright notice in the Description page of Project Settings.


#include "UE_DungeonCompany/Public/DungeonCompanyGameInstance.h"
#include "OnlineSubsystem.h"
#include "OnlineSessionSettings.h"

UDungeonCompanyGameInstance::UDungeonCompanyGameInstance()
{

}
void UDungeonCompanyGameInstance::Init()
{
	if (IOnlineSubsystem* SubSystem = IOnlineSubsystem::Get())
	{
		SessionInterface = SubSystem->GetSessionInterface();
		if (SessionInterface.IsValid())
		{
			//Bind Delegates here
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UDungeonCompanyGameInstance::OnCreateSessionComplete);
		}
	}
}

void UDungeonCompanyGameInstance::OnCreateSessionComplete(FName serverName, bool succeeded)
{
	UE_LOG(LogTemp, Warning, TEXT("OnCreateSessionComplete Succeeded: %d"), succeeded);
}

void UDungeonCompanyGameInstance::JoinServer()
{

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
	SessionInterface->CreateSession(0, FName("MySession"), sessionSettings);
}
