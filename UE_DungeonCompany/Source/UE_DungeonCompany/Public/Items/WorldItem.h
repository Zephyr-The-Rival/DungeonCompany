// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "WorldItem.generated.h"

class UItemData;

UCLASS()
class UE_DUNGEONCOMPANY_API AWorldItem : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWorldItem();
	AWorldItem(UItemData* ItemData);
	virtual void Interact() override;

protected:
	

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UItemData> ItemDataClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UItemData* MyData;

	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
