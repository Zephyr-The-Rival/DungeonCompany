// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/ClassSwitcher.h"

#include "DCGame/DC_PC.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "WorldActors/SharedStatsManager.h"


// Sets default values
AClassSwitcher::AClassSwitcher()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bInteractOnServer=true;
}

// Called when the game starts or when spawned
void AClassSwitcher::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AClassSwitcher::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AClassSwitcher::OnHovered(APlayerCharacter* PlayerCharacter)
{
	ASharedStatsManager* wallet = Cast<ASharedStatsManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASharedStatsManager::StaticClass()));
	
	if (wallet->Money < this->Price)
		this->InteractPromptText="Not enough money (25)";
	else
		this->InteractPromptText="Hire class (25)";
	
	if(PlayerCharacter->IsA(ClassToSwitchTo))
		this->InteractPromptText="Already this class";
	
	Super::OnHovered(PlayerCharacter);
	
}

void AClassSwitcher::AuthorityInteract(APawn* InteractingPawn)
{

	if(InteractingPawn->IsA(ClassToSwitchTo))
		return;
	
	ASharedStatsManager* wallet = Cast<ASharedStatsManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ASharedStatsManager::StaticClass()));

	if(wallet->Money< this->Price)
		return;
	
	wallet->Money-=Price;
	wallet->OnMoneyChanged.Broadcast();
	
	Super::Interact(InteractingPawn);
	Cast<ADC_PC>(InteractingPawn->GetController())->SwitchPlayerCharacterClass(this->ClassToSwitchTo);
}

