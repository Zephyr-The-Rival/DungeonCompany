// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Spawners/CatBurglarSpawnVolume.h"

#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Entities/CatBurglar.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "Items/WorldItem.h"

#include "Components/BrushComponent.h"
#include "Engine/StaticMeshActor.h"
#include "Items/Torch_Data.h"

ACatBurglarSpawnVolume::ACatBurglarSpawnVolume(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	static ConstructorHelpers::FClassFinder<ACatBurglar> BPClass(
		TEXT("/Game/_DungeonCompanyContent/Code/Entities/BP_CatBurglar"));
	CatBurglarClass = BPClass.Class;

	static ConstructorHelpers::FObjectFinder<UStaticMesh> NestMeshFile(TEXT("/Script/Engine.StaticMesh'/Game/_DungeonCompanyContent/Assets/Enemies/Spawners/CatBurglar/CatB_nest.CatB_nest'"));
	NestMesh = NestMeshFile.Object;

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NestBurnEffectFile(TEXT("/Script/Niagara.NiagaraSystem'/Game/_DungeonCompanyContent/Assets/VFX/NiagaraSystem/NS_Burglar_Burn.NS_Burglar_Burn'"));
	NestBurnEffect = NestBurnEffectFile.Object;

	PrimaryActorTick.bCanEverTick = true;
}

void ACatBurglarSpawnVolume::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SpawnTransform.SetScale3D(FVector(1, 1, 1) / GetActorScale3D());

	FVector meshLocation = GetWorldSpawnLocation() - FVector::UpVector * 50.f;

	if (!Nest)
	{
		FActorSpawnParameters spawnParams;
		Nest = GetWorld()->SpawnActor<AStaticMeshActor>(meshLocation,
		                                                SpawnTransform.GetRotation().Rotator());
		Nest->GetStaticMeshComponent()->SetStaticMesh(NestMesh);
	}
	else
	{
		Nest->GetStaticMeshComponent()->SetStaticMesh(NestMesh);

		Nest->SetActorLocation(meshLocation);
		Nest->SetActorRotation(SpawnTransform.GetRotation().Rotator());
	};

	Nest->GetStaticMeshComponent()->SetCollisionProfileName("OverlapAll");
	
#if WITH_EDITOR
	Nest->SetLockLocation(true);
	GEngine->OnLevelActorDeleted().RemoveAll(this);
	GEngine->OnLevelActorDeleted().AddUObject(this, &ACatBurglarSpawnVolume::OnLevelActorDeleted);
#endif
	
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

#if WITH_EDITOR
void ACatBurglarSpawnVolume::OnLevelActorDeleted(AActor* DeletedActor)
{
	if(DeletedActor != this)
		return;
	
	if(Nest)
		Nest->Destroy();
}
#endif

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
	if(!IsActorANestBlocker(OtherActor))
		return;

	BlockersInNest.Add(OtherActor);
	
	bNestBlocked = true;

	if(NestBurnEffect && !NestBurnNiagaraComponent)
		NestBurnNiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), NestBurnEffect, Nest->GetActorLocation(), Nest->GetActorRotation(), FVector(1));
}

void ACatBurglarSpawnVolume::OnBurglarNestEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	BlockersInNest.Remove(OtherActor);

	if(BlockersInNest.Num() > 0)
		return;
	
	bNestBlocked = false;
	if(IsValid(NestBurnNiagaraComponent))
	{
		NestBurnNiagaraComponent->DestroyComponent();
		NestBurnNiagaraComponent = nullptr;
	}
}

bool ACatBurglarSpawnVolume::IsActorANestBlocker(AActor* InActor)
{
	AWorldItem* worldItem = Cast<AWorldItem>(InActor);

	if(!worldItem)
		return false;	
	
	UTorch_Data* torchData = Cast<UTorch_Data>(worldItem->GetMyData());

	if(!torchData)
		return false;
	
	return torchData->bOn;
}
