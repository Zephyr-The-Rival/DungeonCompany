// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/WorldCurrency_Data.h"

#include "Kismet/KismetStringLibrary.h"

void UWorldCurrency_Data::DeserializeMyData_Implementation(const FString& StringData)
{
	Super::DeserializeMyData_Implementation(StringData);
	this->PickUpValue = UKismetStringLibrary::Conv_StringToInt(StringData);
}

FString UWorldCurrency_Data::SerializeMyData_Implementation()
{
	return UKismetStringLibrary::Conv_IntToString(this->PickUpValue); 
}
