// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BreakableProp.generated.h"

class AWeapon;
UCLASS(Blueprintable)
class UE_DUNGEONCOMPANY_API ABreakableProp : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABreakableProp();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintNativeEvent)
	void Hit(FVector ImpulseDirection, TSubclassOf<AWeapon> WeaponClass);
	void Hit_Implementation(FVector ImpulseDirection, TSubclassOf<AWeapon> WeaponClass);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void SetBrokenState();
	void SetBrokenState_Implementation();

public:

	//save stuff
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated)
	bool bIsBroken;

protected:
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
};
