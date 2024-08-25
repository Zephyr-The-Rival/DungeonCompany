// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "SessionSubsystem.generated.h"

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
		PlayerCountString = FString(FString::FromInt(CurrentPlayers) + "/" + FString::FromInt(MaxPlayers));
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerDel, const TArray<FServerInfo>&, ServerList);

UCLASS()
class UE_DUNGEONCOMPANY_API USessionSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	USessionSubsystem();

private:
	UPROPERTY(BlueprintAssignable)
	FServerDel SearchComplete;

	IOnlineSessionPtr SessionInterface;

	TSharedPtr<FOnlineSessionSearch> SessionSearch;

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

protected:
	virtual void OnCreateSessionComplete(FName SessionName, bool Succeeded);
	virtual void OnFindSessionComplete(bool Succeeded);
	virtual void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);
	virtual void OnSessionUserInviteAccepted(const bool bWasSuccessful, const int32 ControllerId,
	                                         FUniqueNetIdPtr UserId, const FOnlineSessionSearchResult& InviteResult);

public:
	UFUNCTION(BlueprintCallable)
	void CreateServer(FString ServerName, FString HostName, bool bIsPrivate);

	UFUNCTION(BlueprintCallable)
	void FindServers();

	UFUNCTION(BlueprintCallable)
	void JoinServer(int32 Index);

	UFUNCTION(BlueprintCallable)
	void DestroyCurrentSession();

private:
	TSubclassOf<UUserWidget> LoadingScreen;

private:
	void OnSessionLeft(FName SessionName, bool bWasSuccessful);
};
