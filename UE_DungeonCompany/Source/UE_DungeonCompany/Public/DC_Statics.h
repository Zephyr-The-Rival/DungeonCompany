// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "DC_Statics.generated.h"

#define LogWarning(Message) UE_LOG(LogTemp, Warning, Message)
#define PrintMessage(Message, DisplayLength) GEngine->AddOnScreenDebugMessage(-1, DisplayLength, FColor::Green, Message)

UCLASS()
class UE_DUNGEONCOMPANY_API UDC_Statics : public UObject
{
	GENERATED_BODY()
	
};
