// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_PostMortemPawn.h"
#include "PlayerCharacter/PlayerCharacter.h"

#include "Net/VoiceConfig.h"
#include "EngineUtils.h"

// Sets default values
ADC_PostMortemPawn::ADC_PostMortemPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	VOIPTalker = CreateDefaultSubobject<UVOIPTalker>(TEXT("VOIPTalker"));

}

// Called when the game starts or when spawned
void ADC_PostMortemPawn::BeginPlay()
{
	Super::BeginPlay();

	//VOIPTalker->RegisterWithPlayerState(GetPlayerState());

	if(!IsLocallyControlled())
		return;

	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		APlayerCharacter* currentPlayer = *It;

		if(currentPlayer->IsDead())
			continue;

		PlayerCharacters.Add(currentPlayer);
		currentPlayer->OnPlayerDeath.AddDynamic(this, &ADC_PostMortemPawn::OnPlayerDied);
	}
		
}

void ADC_PostMortemPawn::OnPlayerDied(ADC_Entity* DeadPlayer)
{
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(DeadPlayer);

	if(!playerCharacter)
		return;

	bool bSpecatingDeadPlayer = PlayerCharacters[SpecatingPlayerIndex] == playerCharacter;

	PlayerCharacters.Remove(playerCharacter);

	if(bSpecatingDeadPlayer)
		SpectateLastPlayer();
		

}

// Called every frame
void ADC_PostMortemPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ADC_PostMortemPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ADC_PostMortemPawn::SpectatePlayer(APlayerCharacter* InSpecatingPlayer)
{
	if (InSpecatingPlayer->IsDead())
	{
		SpectateLastPlayer();
		return;
	}

	if (HasAuthority())
		Client_SpectatePlayer(InSpecatingPlayer);
	else
		Server_SpectatePlayer(InSpecatingPlayer);

	AttachToActor(InSpecatingPlayer, FAttachmentTransformRules::KeepRelativeTransform);
}

void ADC_PostMortemPawn::Server_SpectatePlayer_Implementation(APlayerCharacter* InSpecatingPlayer)
{
	AttachToActor(InSpecatingPlayer, FAttachmentTransformRules::KeepRelativeTransform);
}

void ADC_PostMortemPawn::Client_SpectatePlayer_Implementation(APlayerCharacter* InSpecatingPlayer)
{
	AttachToActor(InSpecatingPlayer, FAttachmentTransformRules::KeepRelativeTransform);
}

void ADC_PostMortemPawn::SpectateLastPlayer()
{
	--SpecatingPlayerIndex;

	if(SpecatingPlayerIndex < 0)
		SpecatingPlayerIndex = PlayerCharacters.Num() - 1;

	if(SpecatingPlayerIndex < 0)
		return;

	SpectatePlayer(PlayerCharacters[SpecatingPlayerIndex]);
}

void ADC_PostMortemPawn::SpectateNextPlayer()
{
	++SpecatingPlayerIndex;

	if (SpecatingPlayerIndex >= PlayerCharacters.Num())
		SpecatingPlayerIndex = 0;

	if (SpecatingPlayerIndex >= PlayerCharacters.Num())
		return;

	SpectatePlayer(PlayerCharacters[SpecatingPlayerIndex]);
}

