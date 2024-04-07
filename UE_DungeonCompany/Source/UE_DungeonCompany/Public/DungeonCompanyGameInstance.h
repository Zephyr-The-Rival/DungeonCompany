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
	int32 currentPlayers;
	UPROPERTY(BlueprintReadOnly)
	int32 MaxPlayers;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FServerDel, FServerInfo, ServerListDel);

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
	UPROPERTY(BlueprintAssignable)
	FServerDel ServerListDel;

	IOnlineSessionPtr sessionInterface;

	TSharedPtr<FOnlineSessionSearch> sessionSearch;

	virtual void Init() override;

	virtual void OnCreateSessionComplete(FName serverName, bool succeeded);
	virtual void OnFindSessionComplete(bool succeeded);
	virtual void OnJoinSessionComplete(FName sessionName, EOnJoinSessionCompleteResult::Type result);

	UFUNCTION(BlueprintCallable)
	void CreateServer();

	UFUNCTION(BlueprintCallable)
	void JoinServer();

};
