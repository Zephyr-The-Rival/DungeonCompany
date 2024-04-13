// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "DungeonCompanyGameInstance.generated.h"


USTRUCT(BlueprintType)
struct FServerInfo
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadOnly)
	FString ServerName;
	UPROPERTY(BlueprintReadOnly)
	FString PlayerCountString;//wtf is this? no idea why the guy made that
	UPROPERTY(BlueprintReadOnly)
	int32 currentPlayers;
	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers;
	UPROPERTY(BlueprintReadOnly)
	int32 ArrayIndex;

	void SetPlayerCount()
	{
		PlayerCountString= FString(FString::FromInt(currentPlayers) + "/" + FString::FromInt(MaxPlayers));
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerDel, const TArray<FServerInfo>&, ServerList);



/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UDungeonCompanyGameInstance : public UGameInstance
{
	GENERATED_BODY()

public: 
	UDungeonCompanyGameInstance();
protected:

	FName MySessionName="Hallo";

	UPROPERTY(BlueprintAssignable)
	FServerDel SearchComplete;

	IOnlineSessionPtr sessionInterface;

	TSharedPtr<FOnlineSessionSearch> sessionSearch;

	virtual void Init() override;

	virtual void OnCreateSessionComplete(FName sessionName, bool succeeded);
	virtual void OnFindSessionComplete(bool succeeded);
	virtual void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);

	UFUNCTION(BlueprintCallable)
	void CreateServer(FString serverName, FString hostName);

	UFUNCTION(BlueprintCallable)
	void FindServers();


	UFUNCTION(BlueprintCallable)
	void JoinServer(int32 index);
};
