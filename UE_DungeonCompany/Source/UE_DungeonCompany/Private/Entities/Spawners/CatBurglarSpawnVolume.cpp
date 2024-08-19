// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Spawners/CatBurglarSpawnVolume.h"
#include "Entities/CatBurglar.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "Items/WorldItem.h"

#include "Components/BrushComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Items/ItemData.h"

ACatBurglarSpawnVolume::ACatBurglarSpawnVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<ACatBurglar> BPClass(
		TEXT("/Game/_DungeonCompanyContent/Code/Entities/BP_CatBurglar"));
	CatBurglarClass = BPClass.Class;

	static ConstructorHelpers::FClassFinder<AWorldItem> TorchClass(
		TEXT("/Game/_DungeonCompanyContent/Code/Items/Torch/BP_Torch_World"));
	NestBlockerClasses.Add(TorchClass.Class);

	PrimaryActorTick.bCanEverTick = true;
}

void ACatBurglarSpawnVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SpawnTransform.SetScale3D(FVector(1, 1, 1) / GetActorScale3D());	

	if (!Nest)
	{
		FActorSpawnParameters spawnParams;
		Nest = GetWorld()->SpawnActor<AStaticMeshActor>(GetWorldSpawnLocation(),
		                                                SpawnTransform.GetRotation().Rotator());

		Nest->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
	}
	else
	{
		Nest->SetActorLocation(GetWorldSpawnLocation());
		Nest->SetActorRotation(SpawnTransform.GetRotation().Rotator());
		Nest->SetActorScale3D(FVector(1, 1, 1));
	};

	Nest->GetStaticMeshComponent()->SetCollisionProfileName("OverlapAll");
	Nest->SetLockLocation(true);
}

void ACatBurglarSpawnVolume::BeginPlay()
{
	Super::BeginPlay();

	if (!HasAuthority())
	{
		GetBrushComponent()->SetCollisionProfileName("NoCollision");
		SetActorTickEnabled(false);
		return;
	}
	
	GetBrushComponent()->SetCollisionProfileName("Trigger");
	SetActorTickEnabled(true);

	Nest->OnActorBeginOverlap.AddDynamic(this, &ACatBurglarSpawnVolume::OnBurglarNestBeginOverlap);
	Nest->OnActorEndOverlap.AddDynamic(this, &ACatBurglarSpawnVolume::OnBurglarNestEndOverlap);

	Nest->GetStaticMeshComponent()->SetGenerateOverlapEvents(true);
}

void ACatBurglarSpawnVolume::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (SpawnedCatBurglars.Num() >= MaxCatBurglar || bNestBlocked)
		return;

	int playersNum = PlayerCharactersInVolume.Num();

	APlayerCharacter* playerClosestToSpawnPoint = nullptr;
	float playerClosestDistance = -1.f;

	for (int i = 0; i < playersNum; ++i)
	{
		APlayerCharacter* currentPlayer = PlayerCharactersInVolume[i];

		if (!IsValid(currentPlayer))
			continue;

		float currentDistance = (currentPlayer->GetActorLocation() - GetWorldSpawnLocation()).Length();
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
	bBurglarsCanBeDespawned = false;

	GetWorld()->GetTimerManager().ClearTimer(DespawnBurglarHandle);
}

void ACatBurglarSpawnVolume::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(OtherActor);

	if (!playerCharacter)
		return;

	PlayerCharactersInVolume.Remove(playerCharacter);

	if (PlayerCharactersInVolume.Num() < 1)
	{
		GetWorld()->GetTimerManager().SetTimer(DespawnBurglarHandle, this,
		                                       &ACatBurglarSpawnVolume::MarkBurglarsAsDespawnable,
		                                       VolumeLeftBurglarDespawnDelay);
	}
}

void ACatBurglarSpawnVolume::MarkBurglarsAsDespawnable()
{
	bBurglarsCanBeDespawned = true;
}

void ACatBurglarSpawnVolume::DespawnIdleBurglars()
{
	int catBurglarsNum = SpawnedCatBurglars.Num();

	for (int i = 0; i < catBurglarsNum; ++i)
		DespawnBurglarIfOnIdle(SpawnedCatBurglars[i]);
}

void ACatBurglarSpawnVolume::DespawnBurglarIfOnIdle(ACatBurglar* InCatBurglar)
{
	if (InCatBurglar->GetCurrentBehaviorState() > ECatBurglarBehaviorState::Harassing || InCatBurglar->
		IsVisibleToPlayers())
		return;

	SpawnedCatBurglars.Remove(InCatBurglar);
	InCatBurglar->Destroy();
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

void ACatBurglarSpawnVolume::OnBurglarNestBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	int nestBlockersNum = NestBlockerClasses.Num();
	
	if(!IsActorANestBlocker(OtherActor))
		return;

	BlockersInNest.Add(OtherActor);
	bNestBlocked = true;

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Blocked!"));
}

void ACatBurglarSpawnVolume::OnBurglarNestEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	BlockersInNest.Remove(OtherActor);

	if(BlockersInNest.Num() < 1)
	{
		bNestBlocked = false;
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Unblocked!"));

	}
}

bool ACatBurglarSpawnVolume::IsActorANestBlocker(AActor* InActor) const
{
	int nestBlockersNum = NestBlockerClasses.Num();

	AWorldItem* worldItem = Cast<AWorldItem>(InActor);

	if(!worldItem)
		return false;

	bool isBlockingType = false;

	for (int i = 0; i < nestBlockersNum; ++i)
	{
		if (InActor->IsA(NestBlockerClasses[i]))
		{
			isBlockingType = true;
			break;
		}
	}

	if(!isBlockingType)
		return false;
	
	UItemData* itemData = worldItem->GetMyData();
	TArray<FString> dataArray = itemData->SeperateStringData(itemData->SerializeMyData());

	if(dataArray.Num() < 2)
		return false;
	
	return dataArray[1] == "true";
}
