// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "Items/ItemData.h"

#include "ItemSocket.generated.h"


class APlayerCharacter;

UCLASS(Blueprintable)
class UE_DUNGEONCOMPANY_API AItemSocket : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemSocket();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UItemData> ItemToSacrifice;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	//interact stuff
	//virtual void Interact(APawn* InteractingPawn) override;
	virtual void Interact(APawn* InteractingPawn) override;

	virtual void OnHovered(APlayerCharacter* PlayerCharacter) override;
public:

	UFUNCTION(BlueprintNativeEvent)
	void OnItemPlaced();
	void OnItemPlaced_Implementation();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite)
	bool bUseable=true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bOneTimeUse;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

};
