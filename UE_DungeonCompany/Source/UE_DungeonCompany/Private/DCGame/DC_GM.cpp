// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_GM.h"
#include "DCGame/DC_PC.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "Entities/FunGuy.h"

#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"

ADC_GM::ADC_GM()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = ADC_PC::StaticClass();

}

void ADC_GM::BeginPlay()
{
	Super::BeginPlay();

}

void ADC_GM::Respawn(AController* Controller)
{
	if(!IsValid(Controller))
		return;

	if (APawn* currentPawn = Controller->GetPawn())
		currentPawn->Destroy();
	
	AActor* playerStart = ChoosePlayerStart(Controller);
	
	if (!playerStart) 
	{
		FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &ADC_GM::Respawn, Controller);
		FTimerHandle RespawnTimerHandle;
	
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, 0.1f, false);
		return;
	}
	
	APlayerCharacter* newCharacter = GetWorld()->SpawnActor<APlayerCharacter>(DefaultPawnClass, playerStart->GetActorLocation(), playerStart->GetActorRotation());
	
	if (!newCharacter)
	{
		FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &ADC_GM::Respawn, Controller);
		FTimerHandle RespawnTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, 0.1f, false);
		return;
	}
	
	Controller->ClientSetRotation(playerStart->GetActorRotation());
	Controller->Possess(newCharacter);

}

template<class T>
T* ADC_GM::RandomlySpawnAIEntity(UClass* Class) const
{
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());

	if (!navSys)
		return nullptr;

	T* newAIEntity = nullptr;

	for (int i = 0; i < 5 && !newAIEntity; ++i)
	{
		FNavLocation location;

		if (!navSys->GetRandomPoint(location))
			return nullptr;

		float halfHeight = Class->GetDefaultObject<T>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		newAIEntity = SpawnAIEntity<T>(Class, location.Location + FVector::UpVector * halfHeight);
	}

	return newAIEntity;
}

template<class T>
T* ADC_GM::SpawnAIEntityCloseToActor(UClass* Class, AActor* Actor, float Radius) const
{
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());

	FNavLocation location;

	if (!navSys || !IsValid(Actor))
		return nullptr;

	T* newAIEntity = nullptr;

	for (int i = 0; i < 5 && !newAIEntity; ++i)
	{
		if(!navSys->GetRandomReachablePointInRadius(Actor->GetActorLocation(), Radius, location))
			return nullptr;

		float halfHeight = Class->GetDefaultObject<T>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		newAIEntity = SpawnAIEntity<T>(Class, location.Location + FVector::UpVector * halfHeight);
	}

	return newAIEntity;
}

template<class T>
T* ADC_GM::SpawnAIEntity(UClass* Class, FVector Location) const
{
	UE_LOG(LogTemp, Log, TEXT("Spawning %s at Location: %s"), *T::StaticClass()->GetName(), *Location.ToString());

	return GetWorld()->SpawnActor<T>(Class, Location, FRotator::ZeroRotator);

}