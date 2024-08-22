// Fill out your copyright notice in the Description page of Project Settings.

#include "InputFunctionLibrary.h"
#include "DCGame/DC_PC.h"

#include "InputActionValue.h"
#include "Kismet/KismetMathLibrary.h"

void UInputFunctionLibrary::LookMouse(const FVector2d& Value, APawn* Player)
{
	Player->AddControllerYawInput(Value.X);
	Player->AddControllerPitchInput(Value.Y);
}

void UInputFunctionLibrary::LookGamepad(const FVector2d& Value, APawn* Player)
{
	FVector2D lookVector = Value;

	float lookVectorLength = Value.Length();

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
