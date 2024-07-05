// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/LightBaker.h"
#include "Engine/RendererSettings.h"

// Sets default values
ALightBaker::ALightBaker()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ALightBaker::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALightBaker::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALightBaker::SetShadowProjectSettings(bool bOn)
{
	URendererSettings* Settings = GetMutableDefault<URendererSettings>();
	Settings->bEnableRayTracingShadows=bOn;
	Settings->SaveConfig();
}
