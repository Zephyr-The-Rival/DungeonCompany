// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerHud/PlayerHud.h"
#include "DC_Statics.h"

#include "InputAction.h"

void UPlayerHud::ShowCrosshair_Implementation(const FString& Text, UInputAction* InputAction)
{
	LogWarning(TEXT("UPlayerHud::ShowCrosshair has not been overidden"));

}

void UPlayerHud::HideCrosshair_Implementation()
{
	LogWarning(TEXT("UPlayerHud::HideCrosshair has not been overidden"));
}

void UPlayerHud::ToggleInventory_Implementation(bool On)
{
	LogWarning(TEXT("UPlayerHud::ToggleInventory_Implementation has not been overidden"));
}

void UPlayerHud::RefreshInventory_Implementation()
{
	LogWarning(TEXT("UPlayerHud::RefreshInventory_Implementation has not been overidden"));
}

void UPlayerHud::SwichHandDisplays_Implementation(bool ToHandA)
{
	LogWarning(TEXT("UPlayerHud::SwichHandDisplays_Implementation has not been overidden"));
}

void UPlayerHud::MoveHighlight_Implementation(EDirections direction)
{
	LogWarning(TEXT("UPlayerHud::MoveHighlight_Implementation has not been overidden"));
}

void UPlayerHud::UpdateCrouchIcon_Implementation()
{
	LogWarning(TEXT("UPlayerHud::UpdateCrouchIcon_Implementation has not been overidden"));
}

void UPlayerHud::ToggleOptionsMenu_Implementation(bool On)
{
	LogWarning(TEXT("UPlayerHud::ToggleOptioneMenu_Implementation has not been overidden"));
}


