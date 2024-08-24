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
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	bool bDead = false; 
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	bool bAttacking = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	bool bTookDamage = false;

	UPROPERTY(Transient)
	float LastHP;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
