// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffDebuffBase.generated.h"

class ADC_Entity;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE_DUNGEONCOMPANY_API UBuffDebuffBase : public UActorComponent
{
	GENERATED_BODY()

public:	
	UBuffDebuffBase();

private:
	ADC_Entity* OuterEntity;

public:
	inline ADC_Entity* GetOuterEntity() const { return OuterEntity; }
	
	template<class T>
	T* GetOuterEntity() const { return Cast<T>(OuterEntity); }

protected:
	virtual void BeginPlay() override;
	virtual void Apply();

	virtual void LocalApply();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void Remove();

	virtual void LocalRemove();

public:	
	void Destroy();

private:
	UPROPERTY(EditAnywhere)
	float ActiveSeconds = 0.f;

	FTimerHandle ActiveTimerHandle;

public:
	void Timegate(float TimeSeconds);
		
};
