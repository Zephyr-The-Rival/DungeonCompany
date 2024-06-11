// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "WorldItem.generated.h"

class UItemData;
class APlayerCharacter;

UCLASS()
class UE_DUNGEONCOMPANY_API AWorldItem : public AActor, public IInteractable
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UItemData* MyData;

public:
	inline UItemData* GetMyData() const { return MyData; }

public:
	// Sets default values for this actor's properties
	AWorldItem();
	AWorldItem(UItemData* ItemData);
	virtual void Interact(APawn* InteractingPawn) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnHoldingInHand(bool locallyControlled);

	virtual void OnHoldingInHand_Implementation(bool locallyControlled);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<class UItemData> ItemDataClass;

	UFUNCTION(BlueprintCallable)
	void ActivateMaterialOnTop(UMeshComponent* MeshComponent);

public://attached to hand

	UPROPERTY(Replicated)
	APlayerCharacter* MyCharacterToAttachTo;
	
	void AttachToPlayer();

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
public://item actions

	//can be extended in blueprints
	UFUNCTION(BlueprintNativeEvent)
	void TriggerPrimaryAction(APlayerCharacter* User);
	virtual void TriggerPrimaryAction_Implementation(APlayerCharacter* User);

	UFUNCTION(BlueprintNativeEvent)
	void TriggerSecondaryAction(APlayerCharacter* User);
	virtual void TriggerSecondaryAction_Implementation(APlayerCharacter* User);

public:	//keeping itemdata
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString SerializedStringData;	

};
