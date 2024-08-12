// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Entities/DC_Entity.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig.h"
#include "AIEntity.generated.h"

/**
 * 
 */

class UBehaviorTree;
class APlayerCharacter;
class UBlackboardComponent;

UCLASS()
class UE_DUNGEONCOMPANY_API AAIEntity : public ADC_Entity
{
	GENERATED_BODY()

public:
	AAIEntity();

private:
	UPROPERTY(EditAnywhere, Category = "AI")
	UBehaviorTree* BehaviorTree;

protected:
	UPROPERTY(EditAnywhere, Instanced, Category = "AI|Perception")
	TArray<UAISenseConfig*> SenseConfigs;

	UPROPERTY(EditAnywhere, Category = "AI|Perception")
	TSubclassOf<UAISense> DominantSense;

protected:
	virtual void BeginPlay() override;

public:
	inline UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }
	inline const TArray<UAISenseConfig*>& GetSenseConfigs() const { return SenseConfigs; }
	inline TSubclassOf<UAISense> GetDominantSense() const { return DominantSense; }

protected:
	UPROPERTY(EditAnywhere, Category = "Balancing|Attack")
	float AttackDamage = 20.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Attack")
	float AttackRange = 250.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Attack")
	float AttackRadius = 30.f;

	UPROPERTY(EditAnywhere, Category = "Balancing|Attack")
	float AttackDelay = 0.5f;

private:
	FTimerHandle ExecuteAttackHandle;

public:
	virtual void AttackPlayer(APlayerCharacter* TargetPlayer);

protected:
	virtual void ExecuteAttack(FVector Direction);

public:
	void SetInAttackOnBlackboard(bool InAttack);
	void SetTargetPlayer(APlayerCharacter* TargetPlayer);

public:
	bool IsVisibleToPlayers() const;

protected:
	template <typename T>
	T* GetSenseConfig()
	{
		return Cast<T>(GetSenseConfig(UAISense::GetSenseID<T>()));
	}

	UAISenseConfig* GetSenseConfig(FAISenseID SenseID);
	void UpdatePerception();

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void OnTargetingPlayer(APlayerCharacter* Target);
	virtual void OnTargetingPlayer_Implementation(APlayerCharacter* Target);

public:
	virtual void HandleSenseUpdate(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent);

protected:
	virtual void HandleSightSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent);
	virtual void HandleHearingSense(AActor* Actor, FAIStimulus const Stimulus, UBlackboardComponent* BlackboardComponent);

public:
	virtual void OnDeath_Implementation() override;

public:
	enum EAnimationFlags
	{
		FLAG_Attacking = 0x01,	
		FLAG_Custom_0 = 0x02,	
		FLAG_Custom_1 = 0x04,
		FLAG_Custom_2 = 0x08,
		FLAG_Custom_3 = 0x10,
		FLAG_Custom_4 = 0x20,
		FLAG_Custom_5 = 0x40,
		FLAG_Custom_6 = 0x80,
	};

	inline bool IsAttacking() const { return (AnimationFlags & FLAG_Attacking) != 0; }
	void SetIsAttacking(bool InAttacking);

protected:
	UPROPERTY(Replicated)
	uint8 AnimationFlags = 0;

	void SetAnimationBitFlag(EAnimationFlags InBit);
	void ClearAnimatinoBitFlag(EAnimationFlags InBit);
	void ToggleAnimationBitFlag(EAnimationFlags InBit);

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Sound")
	USoundBase* DeathSound;

private:
	UFUNCTION(NetMulticast, Unreliable)
	void PlayDeathSound();
	void PlayDeathSound_Implementation();
};
