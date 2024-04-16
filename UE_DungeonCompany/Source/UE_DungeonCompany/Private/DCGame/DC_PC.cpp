// Fill out your copyright notice in the Description page of Project Settings.


#include "DCGame/DC_PC.h"

ADC_PC::ADC_PC()
{

}

void ADC_PC::BeginPlay()
{
	Super::BeginPlay();

	FInputModeGameOnly mode;
	this->SetInputMode(mode);

}