// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldItem.h"
#include "Potion.generated.h"

class UBuffDebuffBase;

UCLASS()
class UE_DUNGEONCOMPANY_API APotion : public AWorldItem
{
	GENERATED_BODY()

private:
	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UBuffDebuffBase>> ApplyingBuffs;

public:
	// Sets default values for this actor's properties
	APotion();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	void Authority_ApplyEffect(APlayerCharacter* User);
	
	UFUNCTION(BlueprintNativeEvent)
	void Local_ApplyEffect(APlayerCharacter* User);
	void Local_ApplyEffect_Implementation(APlayerCharacter* User);

protected:
	UFUNCTION(BlueprintNativeEvent)
	void Authority_OnApplyEffect(APlayerCharacter* User);
	void Authority_OnApplyEffect_Implementation(APlayerCharacter* User);

public:
	virtual void TriggerPrimaryAction_Implementation(APlayerCharacter* User) override;
};
