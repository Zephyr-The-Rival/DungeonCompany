// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EOSSubsystem.generated.h"

/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UEOSSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

private:
	FDelegateHandle LoginDelegateHandle;

public:
	void Login();

protected:
	void LoginCompleted(int32 LocalUserNum, bool bWasSuccessful, const FUniqueNetId& UserId, const FString& Error);
	
};
