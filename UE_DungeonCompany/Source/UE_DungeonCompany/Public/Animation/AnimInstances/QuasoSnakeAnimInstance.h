// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstances/AIEntityAnimInstance.h"
#include "QuasoSnakeAnimInstance.generated.h"

/**
 * 
 */

class AQuasoSnake;

UCLASS()
class UE_DUNGEONCOMPANY_API UQuasoSnakeAnimInstance : public UAIEntityAnimInstance
{
	GENERATED_BODY()

private:
	AQuasoSnake* OwningQuaso;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	FVector Velocity = FVector::ZeroVector; 
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	FVector Location = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	bool bLurking = true;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	bool bAttachedToPlayer = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	bool bAttachedFirstPersonPlayer = false;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
};
