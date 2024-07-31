// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/Spawners/SpurchinSpawner.h"
#include "Entities/Spurchin.h"

#include "Components/ArrowComponent.h"

// Sets default values
ASpurchinSpawner::ASpurchinSpawner()
{
	static ConstructorHelpers::FClassFinder<ASpurchin> BPClass(TEXT("/Game/_DungeonCompanyContent/Code/Entities/BP_Spurchin"));
	SpurchinClass = BPClass.Class;

	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));

}

void ASpurchinSpawner::SpawnSpurchin()
{
	GetWorld()->SpawnActor<ASpurchin>(SpurchinClass, GetActorLocation(), GetActorRotation());
}
