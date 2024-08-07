// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactable.h"
#include "WorldItem.generated.h"

class UItemData;
class APlayerCharacter;

class USoundBase;
class UAIPerceptionStimuliSourceComponent;
UCLASS()
class UE_DUNGEONCOMPANY_API AWorldItem : public AActor, public IInteractable
{
	GENERATED_BODY()

private:
	UAIPerceptionStimuliSourceComponent* StimulusSource;

	bool bDroppedByPlayer = false;

public:
	void SetWasDroppedByPlayer(bool InDroppedByPlayer);
	inline bool WasDroppedByPlayer() const { return bDroppedByPlayer; }
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UItemData* MyData;

public:
	inline UItemData* GetMyData() const { return MyData; }

public:
	// Sets default values for this actor's properties
	AWorldItem();
	virtual void Interact(APawn* InteractingPawn) override;

	virtual void OnHovered(APlayerCharacter* PlayerCharacter) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnHoldingInHand(bool LocallyControlled);
	virtual void OnHoldingInHand_Implementation(bool LocallyControlled);

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
	void TriggerLocalPrimaryAction(APlayerCharacter* User);
	virtual void TriggerLocalPrimaryAction_Implementation(APlayerCharacter* User);

	//hold server
	UFUNCTION(BlueprintNativeEvent)
	void TriggerPrimaryActionHold(APlayerCharacter* User);
	virtual void TriggerPrimaryActionHold_Implementation(APlayerCharacter* User);

	UFUNCTION(BlueprintNativeEvent)
	void EndPrimaryActionHold(APlayerCharacter* User);
	virtual void EndPrimaryActionHold_Implementation(APlayerCharacter* User);

	//hold local

	UFUNCTION(BlueprintNativeEvent)
	void TriggerLocalPrimaryActionHold(APlayerCharacter* User);
	virtual void TriggerLocalPrimaryActionHold_Implementation(APlayerCharacter* User);

	UFUNCTION(BlueprintNativeEvent)
	void EndLocalPrimaryActionHold(APlayerCharacter* User);
	virtual void EndLocalPrimaryActionHold_Implementation(APlayerCharacter* User);
	
//secondary
	UFUNCTION(BlueprintNativeEvent)
	void TriggerSecondaryAction(APlayerCharacter* User);
	virtual void TriggerSecondaryAction_Implementation(APlayerCharacter* User);

	UFUNCTION(BlueprintNativeEvent)
	void TriggerLocalSecondaryAction(APlayerCharacter* User);
	virtual void TriggerLocalSecondaryAction_Implementation(APlayerCharacter* User);

public:	//keeping itemdata
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	FString SerializedStringData;	

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* PickUpSound;

public:
	USoundBase* GetPickupSound() const {return this->PickUpSound;}
	
};
