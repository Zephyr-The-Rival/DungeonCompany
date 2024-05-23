// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/WorldItem.h"
#include "Items/ItemData.h"
#include "DC_Statics.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWorldItem::AWorldItem()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	bAlwaysRelevant = true;
}




// Called when the game starts or when spawned
void AWorldItem::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(this->ItemDataClass) && this->MyData==NULL)
		this->MyData = NewObject<UItemData>(GetTransientPackage(), *ItemDataClass);

	if (IsValid(MyCharacterToAttachTo))
	{
		AttachToPlayer();
	}
	
}


void AWorldItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWorldItem, MyCharacterToAttachTo);
}




// Called every frame
void AWorldItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWorldItem::OnHoldingInHand_Implementation()
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
	this->OnHoldingInHand();
	this->AttachToComponent(MyCharacterToAttachTo->GetFirstPersonMesh(), FAttachmentTransformRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, true), "Item_Joint_R");
	this->SetActorScale3D(FVector(1, 1, 1));

}

void AWorldItem::Interact(APawn* InteractingPawn)
{
	LogWarning(*(this->GetName()+" is beeing interacted with"));

	APlayerCharacter* character = Cast<APlayerCharacter>(InteractingPawn);

	if(!character)
		return;

	character->PickUpItem(this);
}

void AWorldItem::TriggerPrimaryAction_Implementation(APlayerCharacter* User)
{
	LogWarning(TEXT("World Item parent primary action was called"));
}
