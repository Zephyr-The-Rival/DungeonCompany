// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/Store.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Items/WorldItem.h"
#include "Items/ItemData.h"
#include "Items/WorldCurrency.h"
#include "Items/BackPack.h"
#include "Kismet/KismetStringLibrary.h"

// Sets default values
AStore::AStore()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	this->DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	this->RootComponent = DefaultSceneRoot;

	

	SellBox = CreateDefaultSubobject<UBoxComponent>(TEXT("SellBoxCollision"));
	SellBox->SetupAttachment(DefaultSceneRoot);

	this->CoinSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("CoinSpawnPoint"));
	CoinSpawnPoint->SetupAttachment(DefaultSceneRoot);
}

// Called when the game starts or when spawned
void AStore::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AStore::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AStore::SellItems()
{
	TArray<AActor*> overlappingActors;
	SellBox->GetOverlappingActors(overlappingActors, AWorldItem::StaticClass());

	int32 sumValue=0;
	for (AActor* a : overlappingActors)
	{
		if (AWorldItem* WI = Cast<AWorldItem>(a))
		{
			if (ABackPack* BP = Cast<ABackPack>(WI))
			{
				for (TSubclassOf<UItemData> data : BP->Items)
				{
					if (IsValid(data))
					{
						UItemData* d = Cast<UItemData>(data->GetDefaultObject(true));
						sumValue += d->GetValue();
					}					
				}
			}
			sumValue += WI->GetMyData()->GetValue();
			WI->Destroy();

		}
	}
	if (sumValue > 0)
	{
		FTransform tmp = CoinSpawnPoint->GetComponentTransform();
		AWorldCurrency* WC = GetWorld()->SpawnActorDeferred<AWorldCurrency>(MoneyBagToSpawn, tmp);
		WC->SerializedStringData = UKismetStringLibrary::Conv_IntToString(sumValue);
		WC->FinishSpawning(tmp);
	}


}

