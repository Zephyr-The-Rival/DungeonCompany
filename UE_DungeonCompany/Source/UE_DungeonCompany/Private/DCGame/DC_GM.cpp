// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_GM.h"
#include "DCGame/DC_PC.h"
#include "PlayerCharacter/PlayerCharacter.h"

ADC_GM::ADC_GM()
{
	DefaultPawnClass = APlayerCharacter::StaticClass();
	PlayerControllerClass = ADC_PC::StaticClass();

}

void ADC_GM::Respawn(AController* Controller)
{
	if(!IsValid(Controller))
		return;

	if (APawn* currentPawn = Controller->GetPawn())
		currentPawn->Destroy();
	
	AActor* playerStart = ChoosePlayerStart(Controller);
	
	if (!playerStart) 
	{
		FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &ADC_GM::Respawn, Controller);
		FTimerHandle RespawnTimerHandle;
	
		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, 0.1f, false);
		return;
	}
	
	APlayerCharacter* newCharacter = GetWorld()->SpawnActor<APlayerCharacter>(DefaultPawnClass, playerStart->GetActorLocation(), playerStart->GetActorRotation());
	
	if (!newCharacter)
	{
		FTimerDelegate RespawnDelegate = FTimerDelegate::CreateUObject(this, &ADC_GM::Respawn, Controller);
		FTimerHandle RespawnTimerHandle;

		GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, RespawnDelegate, 0.1f, false);
		return;
	}
	
	Controller->ClientSetRotation(playerStart->GetActorRotation());
	Controller->Possess(newCharacter);

}
