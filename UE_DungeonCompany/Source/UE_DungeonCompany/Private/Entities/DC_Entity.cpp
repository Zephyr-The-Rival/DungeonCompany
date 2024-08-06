// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/DC_Entity.h"
#include "AI/DC_AIController.h"
#include "BuffSystem/BuffDebuffBase.h"
#include "DC_Statics.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Kismet/GameplayStatics.h"

ADC_Entity::ADC_Entity()
{
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> basicBloodeffect(TEXT("/Game/_DungeonCompanyContent/Assets/VFX/BaseElement/Blood/NS_Blood"));
	this->bloodEffect= basicBloodeffect.Object;

	static ConstructorHelpers::FObjectFinder<USoundBase> basicTakeDamageSound(TEXT("/Game/_DungeonCompanyContent/Audio/TakeDamageSounds/PlayerHit.PlayerHit"));
	this->TakeDamageSound=basicTakeDamageSound.Object;
}

ADC_Entity::ADC_Entity(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void ADC_Entity::SpawnHitEffect_Implementation(USceneComponent* hitComponent, FName BoneName, FVector hitPoint, FVector HitNormal)
{
	if (IsValid(this->bloodEffect))
	{
		UNiagaraComponent* tmp = UNiagaraFunctionLibrary::SpawnSystemAttached(this->bloodEffect, hitComponent, BoneName, hitPoint, FRotator(0.f), EAttachLocation::Type::KeepWorldPosition, true);
		tmp->SetVariableVec3(TEXT("Direction"), HitNormal * -1);
	}
}

void ADC_Entity::CheckIfDead()
{
	if (HP <= 0.f)
		OnDeath();
}

void ADC_Entity::TakeDamage(float Damage)
{
	//blood particle have to be spawned speperately look at this->SpawnHitEffect
	if (HP <= 0.f)
		return;

	UE_LOG(LogTemp, Log, TEXT("Taking damage : %s"), *FString::SanitizeFloat(Damage));

	HP -= Damage;

	if (HP > 0.f)
		return;

	HP = 0.f;

	CheckIfDead();

}

void ADC_Entity::OnDeath_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("%s died!"), *GetName());

	HP = 0.f;
	OnPlayerDeath.Broadcast(this);

}

void ADC_Entity::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADC_Entity, HP);

}

UBuffDebuffBase* ADC_Entity::AddBuffOrDebuff(TSubclassOf<class UBuffDebuffBase> BuffDebuffClass, float ActiveTime /*= 0.f*/)
{
	if (!HasAuthority())
		return nullptr;

	UBuffDebuffBase* ExistingDeBuff = Cast<UBuffDebuffBase>(GetComponentByClass(BuffDebuffClass));

	if (ExistingDeBuff)
	{
		ExistingDeBuff->Timegate(ActiveTime);
		return ExistingDeBuff;
	}

	UBuffDebuffBase* DeBuff = NewObject<UBuffDebuffBase>(this, BuffDebuffClass);

	DeBuff->RegisterComponent();
	DeBuff->Timegate(ActiveTime);

	return DeBuff;
}

void ADC_Entity::RemoveBuffOrDebuff(TSubclassOf<class UBuffDebuffBase> BuffDebuffClass)
{
	if (!HasAuthority())
		return;

	UBuffDebuffBase* ExistingDeBuff = Cast<UBuffDebuffBase>(GetComponentByClass(BuffDebuffClass));

	if (ExistingDeBuff)
		ExistingDeBuff->Destroy();

}

void ADC_Entity::SpawnTakeDamageSound_Implementation()
{
	if (IsValid(this->TakeDamageSound))
	{
		UGameplayStatics::SpawnSoundAtLocation(this, TakeDamageSound, GetActorLocation());
	}
}
