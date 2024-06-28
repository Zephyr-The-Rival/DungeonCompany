// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Store.generated.h"

class UBoxComponent;

UCLASS()
class UE_DUNGEONCOMPANY_API AStore : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AStore();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* SellBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* DefaultSceneRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	USceneComponent* CoinSpawnPoint;

	UFUNCTION(BlueprintCallable)
	void SellItems();


	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class AWorldCurrency> MoneyBagToSpawn;
};
