// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "LobbyInfo.generated.h"

class UPlayerOptions;
class UVerticalBox;
/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API ULobbyInfo : public UCommonActivatableWidget
{
	GENERATED_BODY()

private:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* LobbyOptionsVerticalBox;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UPlayerOptions> PlayerOptionsClass;

protected:
	UPROPERTY(BlueprintReadOnly)
	TMap<APlayerState*, UPlayerOptions*> PlayerOptionsMap;

public:
	ULobbyInfo();

protected:
	virtual void NativeConstruct() override;

	void AddPlayerOptionsForState(APlayerState* PlayerState);
};