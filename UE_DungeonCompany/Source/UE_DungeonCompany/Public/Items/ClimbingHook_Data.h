// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Items/ItemData.h"
#include "ClimbingHook_Data.generated.h"

/**
 * 
 */

class AClimbingHook;

UCLASS()
class UE_DUNGEONCOMPANY_API UClimbingHook_Data : public UItemData
{
	GENERATED_BODY()

private:
	AClimbingHook* PartnerHook;

public:
	inline AClimbingHook* GetPartnerHook() { return PartnerHook; }
	void SetPartnerHook(AClimbingHook* InPartnerHook);

public:
	virtual FString SerializeMyData_Implementation() override;

	virtual void DeserializeMyData_Implementation(const FString& StringData) override;
	
};
