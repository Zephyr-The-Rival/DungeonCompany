// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "BasicButton.generated.h"

class APlayerCharacter;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnInteractedDelegate, APawn*, InteractingPawn);

UCLASS()
class UE_DUNGEONCOMPANY_API ABasicButton : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABasicButton();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	virtual void Interact(APawn* InteractingPawn) override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;


	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FOnInteractedDelegate OnButtonPressed;
};

