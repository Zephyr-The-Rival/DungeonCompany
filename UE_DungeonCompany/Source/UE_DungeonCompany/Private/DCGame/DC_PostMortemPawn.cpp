// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_PostMortemPawn.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "DCGame/DC_PC.h"

#include "Net/VoiceConfig.h"
#include "EngineUtils.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"

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

}

void ADC_PostMortemPawn::OnPlayerDied(ADC_Entity* DeadPlayer)
{
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(DeadPlayer);

	if(!playerCharacter)
		return;

	bool bSpecatingDeadPlayer = PlayerCharacters[SpecatingPlayerIndex] == playerCharacter;

	if(bSpecatingDeadPlayer)
		SpectateLastPlayer();
		

}

// Called every frame
void ADC_PostMortemPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADC_PostMortemPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	Client_PossessedBy(NewController);
}

void ADC_PostMortemPawn::Client_PossessedBy_Implementation(AController* NewController)
{
	ADC_PC* playerController = Cast<ADC_PC>(NewController);

	if (!playerController)
		return;

	OnInputDeviceChanged(playerController->IsUsingGamepad());
	playerController->OnInputDeviceChanged.AddDynamic(this, &ADC_PostMortemPawn::OnInputDeviceChanged);

	ULocalPlayer* localPlayer = playerController->GetLocalPlayer();

	if (!localPlayer)
		return;

	UEnhancedInputLocalPlayerSubsystem* inputLocalPlayer = localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (!inputLocalPlayer)
		return;

	localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->AddMappingContext(PMPawnInputMapping, 0);

	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		APlayerCharacter* currentPlayer = *It;

		if (currentPlayer->IsDead())
			continue;

		PlayerCharacters.Add(currentPlayer);
		currentPlayer->OnPlayerDeath.AddDynamic(this, &ADC_PostMortemPawn::OnPlayerDied);
	}
}

// Called to bind functionality to input
void ADC_PostMortemPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent);

	if (!EIC)
		return;

	EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADC_PostMortemPawn::Look);
	EIC->BindAction(LookAction, ETriggerEvent::None, this, &ADC_PostMortemPawn::NoLook);

	EIC->BindAction(SpectateLastAction, ETriggerEvent::Started, this, &ADC_PostMortemPawn::SpectateLastPlayer);
	EIC->BindAction(SpectateNextAction, ETriggerEvent::Started, this, &ADC_PostMortemPawn::SpectateNextPlayer);

}

void ADC_PostMortemPawn::SpectatePlayer(APlayerCharacter* InSpecatingPlayer)
{
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

void ADC_PostMortemPawn::OnInputDeviceChanged(bool IsUsingGamepad)
{
	LookFunction = IsUsingGamepad ? &UInputFunctionLibrary::LookGamepad : &UInputFunctionLibrary::LookMouse;
}

void ADC_PostMortemPawn::Look(const FInputActionValue& Value)
{
	(*LookFunction)(Value, this);
}

void ADC_PostMortemPawn::NoLook()
{
	auto pc = Cast<ADC_PC>(Controller);

	if (!pc)
		return;

	pc->SetLastLookVectorLength(0.f);
}

void ADC_PostMortemPawn::SpectateLastPlayer()
{
	--SpecatingPlayerIndex;
	int playersNum = PlayerCharacters.Num();

	for (int i = 0; i < playersNum; ++i, --SpecatingPlayerIndex)
	{
		if (SpecatingPlayerIndex < 0)
			SpecatingPlayerIndex = playersNum - 1;

		if(!PlayerCharacters[SpecatingPlayerIndex]->IsDead())
			break;
	}

	if (SpecatingPlayerIndex < 0)
		return;

	SpectatePlayer(PlayerCharacters[SpecatingPlayerIndex]);
}

void ADC_PostMortemPawn::SpectateNextPlayer()
{
	++SpecatingPlayerIndex;
	int playersNum = PlayerCharacters.Num();

	for (int i = 0; i < playersNum; ++i, ++SpecatingPlayerIndex)
	{
		if (SpecatingPlayerIndex >= playersNum)
			SpecatingPlayerIndex = 0;

		if (!PlayerCharacters[SpecatingPlayerIndex]->IsDead())
			break;
	}

	if (SpecatingPlayerIndex >= playersNum)
		return;

	SpectatePlayer(PlayerCharacters[SpecatingPlayerIndex]);
}

