// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_GM.h"
#include "DCGame/DC_PC.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "Entities/FunGuy.h"
#include "Entities/QuasoSnake.h"
#include "Entities/Spawners/SpurchinSpawner.h"
#include "DCGame/DC_PostMortemPawn.h"

#include "NavigationSystem.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "EngineUtils.h"
#include "DCGame/DC_PS.h"
#include "Items/ItemData.h"
#include "Items/WorldItem.h"

template <class T>
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

template <class T>
T* ADC_GM::SpawnAIEntityCloseToActor(UClass* Class, AActor* Actor, float Radius) const
{
	UNavigationSystemV1* navSys = UNavigationSystemV1::GetCurrent(GetWorld());

	FNavLocation location;

	if (!navSys || !IsValid(Actor))
		return nullptr;

	T* newAIEntity = nullptr;

	for (int i = 0; i < 5 && !newAIEntity; ++i)
	{
		if (!navSys->GetRandomReachablePointInRadius(Actor->GetActorLocation(), Radius, location))
			return nullptr;

		float halfHeight = Class->GetDefaultObject<T>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		newAIEntity = SpawnAIEntity<T>(Class, location.Location + FVector::UpVector * halfHeight);
	}

	return newAIEntity;
}

template <>
AQuasoSnake* ADC_GM::SpawnAIEntityCloseToActor<AQuasoSnake>(UClass* Class, AActor* Actor, float Radius) const
{
	if (!IsValid(Actor))
		return nullptr;

	FHitResult hit;

	FVector start = Actor->GetActorLocation();

	for (int i = 0; i < 10 && !hit.bBlockingHit; ++i)
	{
		float randomNumber = UKismetMathLibrary::RandomFloatInRange(0.f, 62.8f);

		FVector coneDirection = {FMath::Sin(randomNumber), FMath::Cos(randomNumber), 0.25f};
		FVector traceDirection = UKismetMathLibrary::RandomUnitVectorInConeInRadians(
			coneDirection, FMath::DegreesToRadians(20.f));
		FVector end = Radius * traceDirection + start;

		GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_GameTraceChannel3);

		DrawDebugLine(GetWorld(), start, end, FColor::Blue, false, 10.f);
	}

	if (!hit.bBlockingHit)
		return nullptr;

	return SpawnAIEntity<AQuasoSnake>(Class, hit.Location + hit.Normal * 25, hit.Normal.Rotation());
}

template <class T>
T* ADC_GM::SpawnAIEntity(UClass* Class, FVector Location, FRotator Rotation) const
{
	UE_LOG(LogTemp, Log, TEXT("Spawning %s at Location: %s"), *T::StaticClass()->GetName(), *Location.ToString());

	return GetWorld()->SpawnActor<T>(Class, Location, Rotation);
}

ADC_GM::ADC_GM()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = ADC_PC::StaticClass();
	PlayerStateClass = ADC_PS::StaticClass();
}

void ADC_GM::BeginPlay()
{
	Super::BeginPlay();

	SpawnSpurchins();
}

void ADC_GM::SpawnSpurchins()
{
	TArray<ASpurchinSpawner*> allSpurchinSpawner;

	for (TActorIterator<ASpurchinSpawner> It(GetWorld()); It; ++It)
	{
		ASpurchinSpawner* currentSpawner = *It;

		if (currentSpawner->IsSpawnForced())
		{
			currentSpawner->SpawnSpurchin();
			continue;
		}

		allSpurchinSpawner.Add(currentSpawner);
	}

	for (int i = 0; i < SpurchinCount && 0 < allSpurchinSpawner.Num(); ++i)
	{
		int spurchinIndex = FMath::RandRange(0, allSpurchinSpawner.Num() - 1);

		allSpurchinSpawner[spurchinIndex]->SpawnSpurchin();
		allSpurchinSpawner.RemoveAt(spurchinIndex);
	}
}

void ADC_GM::SpawnWorldItem(TSubclassOf<AWorldItem> ItemToSpawn, FTransform SpawnTransform,
                            const FString& SerializedData)
{
	AWorldItem* i = GetWorld()->SpawnActorDeferred<AWorldItem>(ItemToSpawn, SpawnTransform);
	i->SerializedStringData = SerializedData;
	i->FinishSpawning(SpawnTransform);
}

void ADC_GM::Respawn(AController* Controller)
{
	if (!IsValid(Controller))
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

	APlayerCharacter* newCharacter = GetWorld()->SpawnActor<APlayerCharacter>(
		DefaultPawnClass, playerStart->GetActorLocation(), playerStart->GetActorRotation());

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

void ADC_GM::StartSpectating(AController* Controller)
{
	if (!IsValid(Controller))
		return;

	APlayerCharacter* oldPawn = Controller->GetPawn<APlayerCharacter>();
	ADC_PostMortemPawn* postMortemPawn = GetWorld()->SpawnActor<ADC_PostMortemPawn>(PostMortemPawnClass);

	if (!postMortemPawn)
		return;

	Controller->Possess(postMortemPawn);

	if (!oldPawn)
		return;

	postMortemPawn->Client_ForceSpectatePlayer(oldPawn);
}

AAIEntity* ADC_GM::SpawnAIEntity(UClass* Class, AActor* NearActor, float Radius)
{
	if (Class->IsChildOf<AFunGuy>())
	{
		if (NearActor)
			return SpawnAIEntityCloseToActor<AFunGuy>(Class, NearActor, Radius);

		return RandomlySpawnAIEntity<AFunGuy>(Class);
	}
	else if (Class->IsChildOf<AQuasoSnake>())
	{
		if (NearActor)
			return SpawnAIEntityCloseToActor<AQuasoSnake>(Class, NearActor, Radius);

		return RandomlySpawnAIEntity<AQuasoSnake>(Class);
	}

	if (NearActor)
		SpawnAIEntityCloseToActor(Class, NearActor, Radius);

	return RandomlySpawnAIEntity(Class);;
}
