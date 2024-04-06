// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Interfaces/OnlineSessionInterface.h"
#include "DungeonCompanyGameInstance.generated.h"

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
