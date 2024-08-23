// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/WorldItem.h"
#include "Items/ItemData.h"
#include "DC_Statics.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "UI/PlayerHud/PlayerHud.h"
#include "Net/UnrealNetwork.h"
#include "Inventory/InventorySlot.h"

#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"

void AWorldItem::SetWasDroppedByPlayer(bool InDroppedByPlayer)
{
	bDroppedByPlayer = InDroppedByPlayer;
}

// Sets default values
AWorldItem::AWorldItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
	bAttachesToRightHand=true;

	static ConstructorHelpers::FObjectFinder<USoundBase> basicSound(TEXT("/Game/_DungeonCompanyContent/Audio/PickUpSounds/PickUpGeneric"));
	this->PickUpSound = basicSound.Object;
	
	StimulusSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	StimulusSource->RegisterForSense(UAISense_Sight::StaticClass());
	StimulusSource->RegisterWithPerceptionSystem();
	
}

// Called when the game starts or when spawned
void AWorldItem::BeginPlay()
{
	if (IsValid(MyCharacterToAttachTo))
	{
		AttachToPlayer();
		if(IsValid(MyCharacterToAttachTo->GetCurrentlyHeldInventorySlot()->MyItem))
			this->MyData = MyCharacterToAttachTo->GetCurrentlyHeldInventorySlot()->MyItem;//when player spawns item in hand so it doesnt create a new item data
	}

	if (IsValid(this->ItemDataClass) && this->MyData==NULL)
		this->MyData = NewObject<UItemData>(GetTransientPackage(), *ItemDataClass);

	if (!SerializedStringData.IsEmpty())
	{
		MyData->DeserializeMyData(SerializedStringData);
	}
	
	this->bNeedsHolding=this->bNeedsHoldToPickUp;
	this->HoldInteractTime= this->HoldInteractTime;
	
	Super::BeginPlay();
}


void AWorldItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWorldItem, MyCharacterToAttachTo);
	DOREPLIFETIME(AWorldItem, SerializedStringData);
}




// Called every frame
void AWorldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AWorldItem::OnHoldingInHand_Implementation(bool locallyControlled)
{
	LogWarning(*(this->GetName()+"->OnHoldingInHand() was not overridden"));
}

void AWorldItem::ActivateMaterialOnTop(UMeshComponent* MeshComponent)
{

	for (int i = 0; i < MeshComponent->GetNumMaterials(); i++)
	{
		UMaterialInstanceDynamic* materialInstance = MeshComponent->CreateAndSetMaterialInstanceDynamic(i);
		materialInstance->SetScalarParameterValue(TEXT("OnTopActive"), 1);
	}
}

void AWorldItem::AttachToPlayer()
{
	this->OnHoldingInHand(MyCharacterToAttachTo->IsLocallyControlled());

	if (MyCharacterToAttachTo->IsLocallyControlled())
	{
		if(bAttachesToRightHand)
			this->AttachToComponent(MyCharacterToAttachTo->GetFirstPersonMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true), "ItemHandle_R_001");
		else
			this->AttachToComponent(MyCharacterToAttachTo->GetFirstPersonMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true), "ItemHandle_L_001");
	}		
	else
	{	
		this->AttachToComponent(MyCharacterToAttachTo->GetMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true), "ItemHandle_R_001");
	}
	
	this->SetActorScale3D(FVector(1, 1, 1));

}


void AWorldItem::Interact(APawn* InteractingPawn)
{

	APlayerCharacter* character = Cast<APlayerCharacter>(InteractingPawn);

	if(!character)
		return;
	
	character->PickUpItem(this);
}

void AWorldItem::OnHovered(APlayerCharacter* PlayerCharacter)
{
	PlayerCharacter->GetMyHud()->ShowTextInteractPrompt(HoveredMessage);
}

void AWorldItem::TriggerPrimaryAction_Implementation(APlayerCharacter* User)
{
	LogWarning(TEXT("World Item parent primary action was called"));
}

void AWorldItem::TriggerLocalPrimaryAction_Implementation(APlayerCharacter* User)
{
	LogWarning(TEXT("World Item parent local primary action was called"));
}

void AWorldItem::TriggerPrimaryActionHold_Implementation(APlayerCharacter* User)
{
	LogWarning(TEXT("World Item parent primary action hold was called"));
}

void AWorldItem::EndPrimaryActionHold_Implementation(APlayerCharacter* User)
{
	LogWarning(TEXT("World Item parent primary action END hold was called"));
}

void AWorldItem::TriggerLocalPrimaryActionHold_Implementation(APlayerCharacter* User)
{
	LogWarning(TEXT("World Item parent local primary action hold was called"));
}

void AWorldItem::EndLocalPrimaryActionHold_Implementation(APlayerCharacter* User)
{
	LogWarning(TEXT("World Item parent local primary action END hold was called"));
}


void AWorldItem::TriggerSecondaryAction_Implementation(APlayerCharacter* User)
{
	LogWarning(TEXT("World Item parent secondary action was called"));
}

void AWorldItem::TriggerLocalSecondaryAction_Implementation(APlayerCharacter* User)
{
	LogWarning(TEXT("World Item parent local secondary action was called"));
}


