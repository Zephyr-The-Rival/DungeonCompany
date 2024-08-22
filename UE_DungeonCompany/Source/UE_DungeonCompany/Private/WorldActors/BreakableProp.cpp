// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/BreakableProp.h"

#include "DC_Statics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABreakableProp::ABreakableProp()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	bReplicates=true;
}

// Called when the game starts or when spawned
void ABreakableProp::BeginPlay()
{
	Super::BeginPlay();

	if(!HasAuthority()&&bIsBroken)
		SetBrokenState();
}

// Called every frame
void ABreakableProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABreakableProp::Hit_Implementation(FVector ImpulseDirection, TSubclassOf<AWeapon> WeaponClass)
{
	
}

void ABreakableProp::SetBrokenState_Implementation()
{
	FString text= "SetBrokenState() must be overridden";
	LogWarning(*text);
}

void ABreakableProp::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABreakableProp, bIsBroken);
}

