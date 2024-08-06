// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/Actor.h"
#include "SharedStatsManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoneyChanged);
UCLASS()
class UE_DUNGEONCOMPANY_API ASharedStatsManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASharedStatsManager();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

	UPROPERTY(ReplicatedUsing=OnMoneyReplicated, EditAnywhere, BlueprintReadWrite)
	int32 Money=0;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void OnMoneyReplicated();

public:
	
	UPROPERTY(BlueprintAssignable)
	FOnMoneyChanged OnMoneyChanged;
	
};
