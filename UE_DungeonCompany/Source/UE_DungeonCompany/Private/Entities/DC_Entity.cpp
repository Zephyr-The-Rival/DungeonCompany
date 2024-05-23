// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/DC_Entity.h"
#include "AI/DC_AIController.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

ADC_Entity::ADC_Entity()
{

}

ADC_Entity::ADC_Entity(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	
}

void ADC_Entity::TakeDamage(float Damage)
{
	if (HP <= 0.f)
		return;

	UE_LOG(LogTemp, Log, TEXT("Taking damage : %s"), *FString::SanitizeFloat(Damage));

	HP -= Damage;

	if (HP > 0.f)
		return;

	HP = 0.f;

	OnDeath();

}

void ADC_Entity::OnDeath_Implementation()
{
	UE_LOG(LogTemp, Log, TEXT("%s died!"), *GetName());

}

void ADC_Entity::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ADC_Entity, HP);

}
