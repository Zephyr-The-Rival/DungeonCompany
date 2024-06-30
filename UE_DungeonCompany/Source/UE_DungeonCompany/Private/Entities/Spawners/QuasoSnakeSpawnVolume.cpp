// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Spawners/QuasoSnakeSpawnVolume.h"
#include "Entities/QuasoSnake.h"
#include "DC_Statics.h"

#include "Components/BrushComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"

AQuasoSnakeSpawnVolume::AQuasoSnakeSpawnVolume()
{
	static ConstructorHelpers::FClassFinder<AQuasoSnake> BPClass(TEXT("/Game/_DungeonCompanyContent/Code/Entities/BP_QuasoSnake"));
	QuasoSnakeClass = BPClass.Class;

	PrimaryActorTick.bCanEverTick = true;
}

void AQuasoSnakeSpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		GetBrushComponent()->SetCollisionProfileName("NoCollision");
		SetActorTickEnabled(false);
	} 
	else
		SetActorTickEnabled(true);

}

void AQuasoSnakeSpawnVolume::ReturnQuasoSnake()
{
	bSpawnedQuasoSnake = false;
}

void AQuasoSnakeSpawnVolume::EndQuasoSnake(AActor* Actor, EEndPlayReason::Type EndPlayReason)
{
	GetWorld()->GetTimerManager().SetTimer(RespawnHandle, this, &AQuasoSnakeSpawnVolume::ReturnQuasoSnake, RespawnSeconds);
}

void AQuasoSnakeSpawnVolume::Tick(float DeltaSeconds)
{
	if (IsValid(PlayerSpawningCloseTo) && !bSpawnedQuasoSnake)
	{
		SpawnCloseToPlayer(PlayerSpawningCloseTo);
		return;
	}

	if (!IsValid(PlayerSpawningCloseTo) && bSpawnedQuasoSnake && !bDespawnTimerRunning)
	{
		GetWorld()->GetTimerManager().SetTimer(RespawnHandle, this, &AQuasoSnakeSpawnVolume::DespawnQuasoSnake, bDespawnTimerRunning);

	}
}

void AQuasoSnakeSpawnVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	ACharacter* character = Cast<ACharacter>(OtherActor);
	
	if(!character || PlayerSpawningCloseTo)
		return;

	GetWorld()->GetTimerManager().ClearTimer(DespawnHandle);

	PlayerSpawningCloseTo = character;
}

void AQuasoSnakeSpawnVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (OtherActor != PlayerSpawningCloseTo)
		return;

	PlayerSpawningCloseTo = nullptr;

	TArray<AActor*> overlappingCharacters;

	GetOverlappingActors(overlappingCharacters, ACharacter::StaticClass());

	if(overlappingCharacters.IsEmpty())
		return;

	PlayerSpawningCloseTo = Cast<ACharacter>(overlappingCharacters[0]);

}

void AQuasoSnakeSpawnVolume::SpawnCloseToPlayer(ACharacter* Character)
{
	FHitResult hit;

	FVector start = Character->GetActorLocation();

	bool failedHit = true;

	for (int i = 0; i < 10 && failedHit; ++i)
	{
		float randomNumber = UKismetMathLibrary::RandomFloatInRange(0.f, 62.8f);

		FVector coneDirection = { FMath::Sin(randomNumber), FMath::Cos(randomNumber), 0.25f };
		FVector traceDirection = UKismetMathLibrary::RandomUnitVectorInConeInRadians(coneDirection, FMath::DegreesToRadians(20.f));
		FVector end = MaxSpawnRadiusAroundPlayer * traceDirection + start;

		GetWorld()->LineTraceSingleByChannel(hit, start, end, ECC_GameTraceChannel3);

		DrawDebugLine(GetWorld(), start, end, FColor::Blue, false, 10.f);

		double hitDistance = (hit.Location - start).Length();

		failedHit = !hit.bBlockingHit || (hitDistance < MinSpawnRadiusAroundPlayer && hitDistance > -MinSpawnRadiusAroundPlayer);

		if(failedHit)
			continue;

		for (FConstPlayerControllerIterator iterator = GetWorld()->GetPlayerControllerIterator(); iterator; ++iterator)
		{
			if (UDC_Statics::IsLocationInViewportOfPlayer(iterator->Get(), hit.Location + hit.Normal * 50))
			{
				failedHit = true;
				break;
			}
		}

	}

	if (failedHit)
		return;

	SpawnedQuasoSnake = GetWorld()->SpawnActor<AQuasoSnake>(QuasoSnakeClass, hit.Location + hit.Normal * 50, hit.Normal.Rotation());

	if (!SpawnedQuasoSnake)
		return;

	bSpawnedQuasoSnake = true;

	SpawnedQuasoSnake->OnWantsToReturnToVolume.AddDynamic(this, &AQuasoSnakeSpawnVolume::ReturnQuasoSnake);
	SpawnedQuasoSnake->OnEndPlay.AddDynamic(this, &AQuasoSnakeSpawnVolume::EndQuasoSnake);
}

void AQuasoSnakeSpawnVolume::DespawnQuasoSnake()
{
	if(!IsValid(SpawnedQuasoSnake))
		return;

	SpawnedQuasoSnake->Destroy();
}
