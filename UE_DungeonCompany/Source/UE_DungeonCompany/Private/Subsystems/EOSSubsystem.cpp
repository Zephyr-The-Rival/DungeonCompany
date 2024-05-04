// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystems/EOSSubsystem.h"

#include "OnlineSubsystem.h"
#include "OnlineSubsystemUtils.h"

void UEOSSubsystem::Login()
{
	IOnlineSubsystem* subsystem = Online::GetSubsystem(this->GetWorld());
	IOnlineIdentityPtr identity = subsystem->GetIdentityInterface();

    FUniqueNetIdPtr netId = identity->GetUniquePlayerId(0);

    if (netId != nullptr && identity->GetLoginStatus(0) == ELoginStatus::LoggedIn)
        return;

    LoginDelegateHandle = identity->OnLoginCompleteDelegates->AddUObject(this, &UEOSSubsystem::LoginCompleted);

    FString authType;
    FParse::Value(FCommandLine::Get(), TEXT("AUTH_TYPE="), authType);

    if(authType.IsEmpty())
        return;
    
    UE_LOG(LogTemp, Log, TEXT("Logging into EOS..."));

    if (identity->AutoLogin(0))
        return;
    
    UE_LOG(LogTemp, Warning, TEXT("Failed to login... ")); 
           
    identity->ClearOnLoginCompleteDelegate_Handle(0, LoginDelegateHandle);
    LoginDelegateHandle.Reset();
    
}

void UEOSSubsystem::LoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error)
{
    IOnlineSubsystem* subsystem = Online::GetSubsystem(GetWorld());
    IOnlineIdentityPtr identity = subsystem->GetIdentityInterface();

    if (bWasSuccessful)
        UE_LOG(LogTemp, Log, TEXT("Login callback completed!"))
    else 
        UE_LOG(LogTemp, Warning, TEXT("EOS login failed.")); 

    identity->OnLoginCompleteDelegates->Clear();
    LoginDelegateHandle.Reset();
}
