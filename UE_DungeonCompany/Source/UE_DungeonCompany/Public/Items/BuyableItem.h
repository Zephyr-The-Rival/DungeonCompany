// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "BuyableItem.generated.h"

class UItemData;
UCLASS()
class UE_DUNGEONCOMPANY_API ABuyableItem : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuyableItem();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Price;
	
	virtual void Interact(APawn* InteractingPawn) override;
	virtual void OnHovered(APlayerCharacter* PlayerCharacter) override;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	int32 GetPrice() const { return this->Price; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UItemData> MyItemDataClass;

};
