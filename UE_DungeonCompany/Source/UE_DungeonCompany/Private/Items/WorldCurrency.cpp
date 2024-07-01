// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/WorldCurrency.h"
#include "Net/UnrealNetwork.h"

void AWorldCurrency::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWorldCurrency, Value);
}