// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_PostMortemPawn.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "DCGame/DC_PC.h"

#include "Net/VoiceConfig.h"
#include "EngineUtils.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputSubsystems.h"
#include "../../../../Plugins/EnhancedInput/Source/EnhancedInput/Public/EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/SpectatorHud/SpectatorHud.h"

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

void ADC_PostMortemPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(!IsLocallyControlled())
		return;

	APlayerController* playerController = GetController<APlayerController>();

	if (!playerController)
		return;

	ULocalPlayer* localPlayer = playerController->GetLocalPlayer();

	if (!localPlayer)
		return;

	UEnhancedInputLocalPlayerSubsystem* inputLocalPlayer =  localPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

	if (!inputLocalPlayer)
		return;

	inputLocalPlayer->RemoveMappingContext(PMPawnInputMapping);
}

void ADC_PostMortemPawn::OnPlayerDied(ADC_Entity* DeadPlayer)
{
	APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(DeadPlayer);

	if(!playerCharacter)
		return;

	FTimerHandle stopSpectatingHandle;
	FTimerDelegate stopSpectatingDelegate = FTimerDelegate::CreateUObject(this, &ADC_PostMortemPawn::StopSpectatingPlayer, playerCharacter);

	GetWorld()->GetTimerManager().SetTimer(stopSpectatingHandle, stopSpectatingDelegate, SpectateSwitchAfterDeathDelay, false);
}

void ADC_PostMortemPawn::StopSpectatingPlayer(APlayerCharacter* InPlayer)
{
	if(!IsSpectatingPlayer(InPlayer))
		return;

	SpectatePreviousPlayer();
}

// Called every frame
void ADC_PostMortemPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADC_PostMortemPawn::Restart()
{
	Super::Restart();

	if(!IsLocallyControlled() || !PMPawnInputMapping)
		return;

	ADC_PC* playerController = Cast<ADC_PC>(Controller);

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
		

		PlayerCharacters.Add(currentPlayer);
		currentPlayer->OnPlayerDeath.AddDynamic(this, &ADC_PostMortemPawn::OnPlayerDied);
	}

	if(!MySpectatorHud)
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
	if(!InSpectatingPlayer)
		return;
	
	AttachToActor(InSpectatingPlayer, FAttachmentTransformRules::KeepRelativeTransform);
	OnSpectatingSwitch.Broadcast(this);
}

void ADC_PostMortemPawn::Client_ForceSpectatePlayer_Implementation(APlayerCharacter* InSpectatingPlayer)
{
	int playersNum = PlayerCharacters.Num();

	for (int i = 0; i < playersNum; ++i)
	{
		if(PlayerCharacters[i] != InSpectatingPlayer)
			continue;

		SpecatingPlayerIndex = i;
		break;
	}

	AttachToActor(InSpectatingPlayer, FAttachmentTransformRules::KeepRelativeTransform);
}

bool ADC_PostMortemPawn::IsSpectatingPlayer(APlayerCharacter* InPlayer)
{
	if(SpecatingPlayerIndex < 0 || SpecatingPlayerIndex >= PlayerCharacters.Num())
		return false;

	return InPlayer == PlayerCharacters[SpecatingPlayerIndex];
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

void ADC_PostMortemPawn::SpectatePreviousPlayer()
{
	
	--SpecatingPlayerIndex;
	int playersNum = PlayerCharacters.Num();

	for (int i = 0; i < playersNum; ++i, --SpecatingPlayerIndex)
	{
		if (SpecatingPlayerIndex < 0)
			SpecatingPlayerIndex = playersNum - 1;

		if(IsValid(PlayerCharacters[SpecatingPlayerIndex]) && !PlayerCharacters[SpecatingPlayerIndex]->IsDead())
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
	if(!IsLocallyControlled())
		return;

	this->MySpectatorHud= CreateWidget<USpectatorHud>(GetWorld(),this->SpectatorHudClass);
	this->MySpectatorHud->AddToViewport();
	
}

