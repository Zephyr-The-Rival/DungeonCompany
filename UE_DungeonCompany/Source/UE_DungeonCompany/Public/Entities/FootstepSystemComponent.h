// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Components/ActorComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "FootstepSystemComponent.generated.h"

USTRUCT(Blueprintable)
struct FFootstepFeedback
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* Sound=nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UNiagaraSystem* VFX=nullptr;
	
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class UE_DUNGEONCOMPANY_API UFootstepSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFootstepSystemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float StepsLineTraceLength = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TMap<UPhysicalMaterial*, FFootstepFeedback> FeedbackMap;
	
public:
	
	UFUNCTION(BlueprintCallable, Category = "Footstep")
	void PlayStepsFeedback(FName SocketName);
	
private:
	FHitResult GetFootHitResult(FName SocketName) const;


protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FFootstepFeedback DefaultFeedback;


};
