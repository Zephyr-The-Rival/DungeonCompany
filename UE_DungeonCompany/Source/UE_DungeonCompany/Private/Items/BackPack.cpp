// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/BackPack.h"
#include "Engine.h"
#include "Net/UnrealNetwork.h"


void ABackPack::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABackPack, Items);
	DOREPLIFETIME(ABackPack, ItemDatas);

}
