// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook_Data.h"

#include "Kismet/KismetStringLibrary.h"

void UClimbingHook_Data::SetPartnerHook(AClimbingHook* InPartnerHook)
{
	PartnerHook = InPartnerHook;
}

FString UClimbingHook_Data::SerializeMyData_Implementation()
{
	FString serializedString = UKismetStringLibrary::Conv_Int64ToString((int64)PartnerHook);
	return serializedString;
}

void UClimbingHook_Data::DeserializeMyData_Implementation(const FString& StringData)
{
	FString partnerHook = SeperateStringData(StringData)[0];

	PartnerHook = (AClimbingHook*)UKismetStringLibrary::Conv_StringToInt64(partnerHook);
}
