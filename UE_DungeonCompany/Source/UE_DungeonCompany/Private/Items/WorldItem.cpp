// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/WorldItem.h"
#include "Items/ItemData.h"
#include "DC_Statics.h"
#include "PlayerCharacter/PlayerCharacter.h"

// Sets default values
AWorldItem::AWorldItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


}

AWorldItem::AWorldItem(UItemData* ItemData)
{
	this->MyData = ItemData;
}



// Called when the game starts or when spawned
void AWorldItem::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(this->ItemDataClass) && this->MyData==NULL)
		this->MyData = NewObject<UItemData>(GetTransientPackage(), *ItemDataClass);

}

void AWorldItem::DestroyOnServer_Implementation()
{
	this->Destroy();
}

bool AWorldItem::DestroyOnServer_Validate()
{
	return true;
}

// Called every frame
void AWorldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorldItem::Interact(APawn* InteractingPawn)
{
	LogWarning(*(this->GetName()+" is beeing interacted with"));

	APlayerCharacter* character = Cast<APlayerCharacter>(InteractingPawn);

	if(!character)
		return;

	character->PickUpItem(this);
}

