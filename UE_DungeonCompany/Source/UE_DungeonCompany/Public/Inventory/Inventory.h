// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Inventory.generated.h"

class UInventorySlot;
class UItemData;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_DUNGEONCOMPANY_API UInventory : public UActorComponent
{
	GENERATED_BODY()

public:	


	// Sets default values for this component's properties
	UInventory();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	UPROPERTY(EditAnywhere, BlueprintGetter= GetSlots)
	TArray<UInventorySlot*> Slots;


public:
	UFUNCTION(BlueprintPure, BlueprintInternalUseOnly)
	TArray<UInventorySlot*> GetSlots() const { return Slots; }

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Balancing")
	int32 NumInventorySlots;

	int32 AddItem(UItemData* Item);
	void RemoveItem(UItemData* ItemToRemove);
	void RemoveItem(int32 index);
	
	UItemData* GetItemAtIndex(int32 Index);
	

};
