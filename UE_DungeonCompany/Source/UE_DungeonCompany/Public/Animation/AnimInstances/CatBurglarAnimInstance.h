// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstances/AIEntityAnimInstance.h"
#include "CatBurglarAnimInstance.generated.h"

class ACatBurglar;
/**
 * 
 */
UCLASS()
class UE_DUNGEONCOMPANY_API UCatBurglarAnimInstance : public UAIEntityAnimInstance
{
	GENERATED_BODY()

private:
	ACatBurglar* OwningCarBurglar;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	FVector Velocity = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	float StalkSpeed = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	float RunSpeed = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	float RetrieveSpeed = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Animation Properties")
	bool bSteals = false;
	
private:
	bool bEyesGlowCurrently = false;

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};
