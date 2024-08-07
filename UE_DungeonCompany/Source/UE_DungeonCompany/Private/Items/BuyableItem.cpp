// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/BuyableItem.h"

#include "DC_Statics.h"
#include "Items/BackPack.h"
#include "Items/ItemData.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "UI/PlayerHud/PlayerHud.h"
#include "WorldActors/SharedStatsManager.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ABuyableItem::ABuyableItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	this->bInteractOnServer = false;

}

// Called when the game starts or when spawned
void ABuyableItem::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABuyableItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABuyableItem::Interact(APawn* InteractingPawn)
{
	APlayerCharacter* PC = Cast<APlayerCharacter>(InteractingPawn);
	ASharedStatsManager* wallet = Cast<ASharedStatsManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASharedStatsManager::StaticClass()));
	
	if (wallet->Money < this->Price)
		return;

	if(PC->HasFreeSpace()|| MyItemDataClass== PC->BackpackClass&& !PC->bHasBackPack)
		PC->BuyItem(this);
	

	
}

void ABuyableItem::OnHovered(APlayerCharacter* PlayerCharacter)
{
	PlayerCharacter->GetMyHud()->ShowBuyPrompt(this);
}

