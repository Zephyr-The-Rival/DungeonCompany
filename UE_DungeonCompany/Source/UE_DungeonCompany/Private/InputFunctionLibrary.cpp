// Fill out your copyright notice in the Description page of Project Settings.

#include "InputFunctionLibrary.h"
#include "DCGame/DC_PC.h"

#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"

void UInputFunctionLibrary::LookMouse(const FInputActionValue& Value, APawn* Player)
{
	FVector2D lookVector = Value.Get<FVector2D>();

	Player->AddControllerYawInput(lookVector.X);
	Player->AddControllerPitchInput(lookVector.Y);
}

void UInputFunctionLibrary::LookGamepad(const FInputActionValue& Value, APawn* Player)
{
	FVector2D lookVector = Value.Get<FVector2D>();

	float lookVectorLength = lookVector.Length();

	float deltaSeconds = Player->GetWorld()->GetDeltaSeconds();

	ADC_PC* pc = Player->GetController<ADC_PC>();

	if(!pc)
		return;

	float lastLookVectorLength = pc->GetLastLookVectorLength();

	if (lookVectorLength > pc->GetLastLookVectorLength())
		lookVectorLength = UKismetMathLibrary::FInterpTo(lastLookVectorLength, lookVectorLength, deltaSeconds, pc->GetGamePadAccelerationSpeed());

	pc->SetLastLookVectorLength(lookVectorLength);

	lookVector.Normalize();
	lookVector *= lookVectorLength * deltaSeconds * 100.f;

	Player->AddControllerYawInput(lookVector.X);
	Player->AddControllerPitchInput(lookVector.Y);
}
