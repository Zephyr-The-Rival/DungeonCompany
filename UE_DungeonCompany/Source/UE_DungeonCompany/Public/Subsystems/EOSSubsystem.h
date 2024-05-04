// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "OnlineSubsystem.h"
#include "EOSSubsystem.generated.h"

/**
 * 
 */
 USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadOnly)
	FString ServerName;
	UPROPERTY(BlueprintReadOnly)
	FString PlayerCountString;
	UPROPERTY(BlueprintReadOnly)
	int32 CurrentPlayers = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers = 0;
	UPROPERTY(BlueprintReadOnly)
	int32 ArrayIndex = 0;

	void SetPlayerCount()
	{
		PlayerCountString= FString(FString::FromInt(CurrentPlayers) + "/" + FString::FromInt(MaxPlayers));
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerDel, const TArray<FServerInfo>&, ServerList);

UCLASS()
class UE_DUNGEONCOMPANY_API UEOSSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	IOnlineSessionPtr SessionInterface;
	IOnlineIdentityPtr IdentityInterface;

	UPROPERTY(BlueprintAssignable)
	FServerDel SearchComplete;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;

protected:
	virtual void OnLoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnFindSessionComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void OnDestroySessionCompleted(FName SessionName, bool bWasSuccessful);
	virtual void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId, FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UFUNCTION(BlueprintCallable)
	void Login();

	bool RegisterPlayer(APlayerController* NewPlayer);

	UFUNCTION(BlueprintCallable)
	void CreateSession(FString ServerName, FString HostName);

	UFUNCTION(BlueprintCallable)
	void FindSessions();

	UFUNCTION(BlueprintCallable)
	void JoinSession(int32 Index);

	UFUNCTION(BlueprintCallable)
	void DestroySession();
	
};
