// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_PostMortemPawn.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "PlayerCharacter/Components/DC_VOIPTalker.h"
#include "DCGame/DC_PC.h"
#include "UI/SpectatorHud/SpectatorHud.h"

#include "EngineUtils.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "OnlineSubsystemUtils.h"
#include "Subsystems/VoiceChatSubsystem.h"

ADC_PostMortemPawn::ADC_PostMortemPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	VOIPTalker = CreateDefaultSubobject<UDC_VOIPTalker>(TEXT("VOIPTalker"));
}

void ADC_PostMortemPawn::BeginPlay()
{
	Super::BeginPlay();
}

void ADC_PostMortemPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (!IsLocallyControlled())
		return;

	APlayerController* playerController = GetController<APlayerController>();

	if (!playerController)
		return;

	ULocalPlayer* localPlayer = playerController->GetLocalPlayer();

	if (!localPlayer)
		return;

	UEnhancedInputLocalPlayerSubsystem* inputLocalPlayer = localPlayer->GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>();

	if (!inputLocalPlayer)
		return;

	inputLocalPlayer->RemoveMappingContext(PMPawnInputMapping);
}

void ADC_PostMortemPawn::OnPlayerDied(ADC_Entity* DeadPlayer)
{
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(DeadPlayer);

	if (!playerCharacter)
		return;

	FTimerHandle stopSpectatingHandle;
	FTimerDelegate stopSpectatingDelegate = FTimerDelegate::CreateUObject(
		this, &ADC_PostMortemPawn::StopSpectatingPlayer, playerCharacter);

	GetWorld()->GetTimerManager().SetTimer(stopSpectatingHandle, stopSpectatingDelegate, SpectateSwitchAfterDeathDelay,
	                                       false);
}

void ADC_PostMortemPawn::StopSpectatingPlayer(APlayerCharacter* InPlayer)
{
	if (!IsSpectatingPlayer(InPlayer))
		return;

	SpectatePreviousPlayer();
}

void ADC_PostMortemPawn::OnPlayerStateChanged(APlayerState* NewPlayerState, APlayerState* OldPlayerState)
{
	Super::OnPlayerStateChanged(NewPlayerState, OldPlayerState);

	if (!NewPlayerState)
		return;

	VOIPTalker->RegisterWithPlayerState(NewPlayerState);

	if (IsLocallyControlled())
		return;

	APlayerController* localPlayerController = GetWorld()->GetFirstPlayerController();

	if (!localPlayerController || localPlayerController->GetPawn<APlayerCharacter>())
		GetGameInstance()->GetSubsystem<UVoiceChatSubsystem>()->MutePlayer(NewPlayerState);
}

void ADC_PostMortemPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADC_PostMortemPawn::Restart()
{
	Super::Restart();

	if (!IsLocallyControlled() || !PMPawnInputMapping)
		return;

	ADC_PC* playerController = Cast<ADC_PC>(Controller);

	if (!playerController)
		return;

	OnInputDeviceChanged(playerController->IsUsingGamepad());
	playerController->OnInputDeviceChanged.AddDynamic(this, &ADC_PostMortemPawn::OnInputDeviceChanged);

	ULocalPlayer* localPlayer = playerController->GetLocalPlayer();

	if (!localPlayer)
		return;

	UEnhancedInputLocalPlayerSubsystem* inputLocalPlayer = localPlayer->GetSubsystem<
		UEnhancedInputLocalPlayerSubsystem>();

	if (!inputLocalPlayer)
		return;

	localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>()->AddMappingContext(PMPawnInputMapping, 0);

	for (TActorIterator<APlayerCharacter> It(GetWorld()); It; ++It)
	{
		APlayerCharacter* currentPlayer = *It;


		PlayerCharacters.Add(currentPlayer);
		currentPlayer->OnEntityDeath.AddDynamic(this, &ADC_PostMortemPawn::OnPlayerDied);
	}

	if (!MySpectatorHud)
		this->CreateSpectatorHud();
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

	EIC->BindAction(SpectateLastAction, ETriggerEvent::Started, this, &ADC_PostMortemPawn::SpectatePreviousPlayer);
	EIC->BindAction(SpectateNextAction, ETriggerEvent::Started, this, &ADC_PostMortemPawn::SpectateNextPlayer);
}

void ADC_PostMortemPawn::SpectatePlayer(APlayerCharacter* InSpectatingPlayer)
{
	if (!InSpectatingPlayer)
		return;

	AttachToActor(InSpectatingPlayer, FAttachmentTransformRules::KeepRelativeTransform);
	OnSpectatingSwitch.Broadcast(this);
}

void ADC_PostMortemPawn::Client_ForceSpectatePlayer_Implementation(APlayerCharacter* InSpectatingPlayer)
{
	int playersNum = PlayerCharacters.Num();

	for (int i = 0; i < playersNum; ++i)
	{
		if (PlayerCharacters[i] != InSpectatingPlayer)
			continue;

		SpecatingPlayerIndex = i;
		break;
	}

	AttachToActor(InSpectatingPlayer, FAttachmentTransformRules::KeepRelativeTransform);
}

bool ADC_PostMortemPawn::IsSpectatingPlayer(APlayerCharacter* InPlayer)
{
	if (SpecatingPlayerIndex < 0 || SpecatingPlayerIndex >= PlayerCharacters.Num())
		return false;

	return InPlayer == PlayerCharacters[SpecatingPlayerIndex];
}

void ADC_PostMortemPawn::OnInputDeviceChanged(bool IsUsingGamepad)
{
	LookFunction = IsUsingGamepad ? &UInputFunctionLibrary::LookGamepad : &UInputFunctionLibrary::LookMouse;
}

void ADC_PostMortemPawn::Look(const FInputActionValue& Value)
{
	(*LookFunction)(Value.Get<FVector2d>(), this);
}

void ADC_PostMortemPawn::NoLook()
{
	auto pc = Cast<ADC_PC>(Controller);

	if (!pc)
		return;

	pc->SetLastLookVectorLength(0.f);
}

void ADC_PostMortemPawn::SpectatePreviousPlayer()
{
	--SpecatingPlayerIndex;
	int playersNum = PlayerCharacters.Num();

	for (int i = 0; i < playersNum; ++i, --SpecatingPlayerIndex)
	{
		if (SpecatingPlayerIndex < 0)
			SpecatingPlayerIndex = playersNum - 1;

		if (IsValid(PlayerCharacters[SpecatingPlayerIndex]) && !PlayerCharacters[SpecatingPlayerIndex]->IsDead())
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

		if (IsValid(PlayerCharacters[SpecatingPlayerIndex]) && !PlayerCharacters[SpecatingPlayerIndex]->IsDead())
			break;
	}

	if (SpecatingPlayerIndex >= playersNum)
		return;

	SpectatePlayer(PlayerCharacters[SpecatingPlayerIndex]);
}

void ADC_PostMortemPawn::CreateSpectatorHud()
{
	if (!IsLocallyControlled())
		return;

	this->MySpectatorHud = CreateWidget<USpectatorHud>(GetWorld(), this->SpectatorHudClass);
	this->MySpectatorHud->AddToViewport();
}

void ADC_PostMortemPawn::Destroyed()
{
	Super::Destroyed();
	if (IsValid(MySpectatorHud))
		MySpectatorHud->RemoveFromParent();
}
