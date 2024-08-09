// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Spawners/CatBurglarSpawnVolume.h"
#include "Entities/CatBurglar.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "Components/BrushComponent.h"

ACatBurglarSpawnVolume::ACatBurglarSpawnVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<ACatBurglar> BPClass(
		TEXT("/Game/_DungeonCompanyContent/Code/Entities/BP_CatBurglar"));
	CatBurglarClass = BPClass.Class;

	PrimaryActorTick.bCanEverTick = true;
}

void ACatBurglarSpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		GetBrushComponent()->SetCollisionProfileName("NoCollision");
		SetActorTickEnabled(false);
	}
}

void ACatBurglarSpawnVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	int playersNum = PlayerCharactersInVolume.Num();

	APlayerCharacter* playerClosestToSpawnPoint = nullptr;
	float playerClosestDistance = -1.f;

	for (int i = 0; i < playersNum; ++i)
	{
		APlayerCharacter* currentPlayer = PlayerCharactersInVolume[i];

		if (!IsValid(currentPlayer))
			continue;

		float currentDistance = (currentPlayer->GetActorLocation() - (SpawnLocation + GetActorLocation())).Length();
		if (playerClosestToSpawnPoint && currentDistance > playerClosestDistance)
			continue;

		playerClosestDistance = currentDistance;
		playerClosestToSpawnPoint = currentPlayer;
	}

	if (playerClosestDistance < MinSpawnDistance)
		return;

	SpawnCatBurglarWithPlayerAsTarget(playerClosestToSpawnPoint);
}

void ACatBurglarSpawnVolume::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(OtherActor);

	if (!playerCharacter)
		return;

	PlayerCharactersInVolume.Add(playerCharacter);
}

void ACatBurglarSpawnVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(OtherActor);

	if (!playerCharacter)
		return;

	PlayerCharactersInVolume.Remove(playerCharacter);
}

ACatBurglar* ACatBurglarSpawnVolume::SpawnCatBurglarWithPlayerAsTarget(APlayerCharacter* TargetPlayer)
{
	FTransform spawnTransform;
	spawnTransform.SetLocation(GetWorldSpawnLocation());
	ACatBurglar* catBurglar = GetWorld()->SpawnActor<ACatBurglar>(CatBurglarClass, spawnTransform);

	if (!catBurglar)
		return nullptr;

	catBurglar->SetTargetPlayer(TargetPlayer);
	SpawnedCatBurglars.Add(catBurglar);
	catBurglar->OnEntityDeath.AddDynamic(this, &ACatBurglarSpawnVolume::OnCatBurglarDeath);

	OnCatBurglarCountChanged();

	SetActorTickEnabled(false);

	FTimerDelegate timerDelegate = FTimerDelegate::CreateUObject(this, &AActor::SetActorTickEnabled, true);

	GetWorld()->GetTimerManager().SetTimer(AllowSpawnHandle, timerDelegate, AllowSpawnDelay, false);

	return catBurglar;
}

void ACatBurglarSpawnVolume::OnCatBurglarDeath(ADC_Entity* DeadBurglar)
{
	ACatBurglar* catBurglar = Cast<ACatBurglar>(DeadBurglar);

	if (!catBurglar)
		return;

	SpawnedCatBurglars.Remove(catBurglar);
	OnCatBurglarCountChanged();
}

void ACatBurglarSpawnVolume::OnCatBurglarCountChanged()
{
	int catBurglarNum = SpawnedCatBurglars.Num();

	uint8 newIdleState = catBurglarNum - 1;

	if (newIdleState > 2)
		newIdleState = 2;

	for (int i = 0; i < catBurglarNum; ++i)
	{
		ACatBurglar* currentBurglar = SpawnedCatBurglars[i];

		if (!IsValid(currentBurglar))
			continue;

		currentBurglar->SetIdleBehaviorState((ECatBurglarBehaviorState)newIdleState);
	}
}
