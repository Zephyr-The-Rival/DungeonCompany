// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NiagaraSystem.h"
#include "Components/ActorComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "FootstepSystemComponent.generated.h"


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

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	//Material Surface Inspection
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	float StepsLineTraceLength = 100.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TMap<UPhysicalMaterial*, UNiagaraSystem*> SurfaceVFX;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Effects")
	TMap<UPhysicalMaterial*,USoundBase*> SurfaceSFX;

private:
	UPhysicalMaterial* GetFootMaterial() const;
	void PlayStepsFeedback();
};
