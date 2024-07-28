// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AIEntityAnimInstance.generated.h"

/**
 * 
 */

class AAIEntity;

UCLASS()
class UE_DUNGEONCOMPANY_API UAIEntityAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	AAIEntity* OwningEntity;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	bool bDead = false; 
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	bool bAttacking = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	bool bHit = false;

	UPROPERTY(Transient)
	float LastHP;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
