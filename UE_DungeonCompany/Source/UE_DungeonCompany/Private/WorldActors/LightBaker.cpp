// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActors/LightBaker.h"
#include "Engine/RendererSettings.h"


void ALightBaker::SetShadowProjectSettings(bool bOn)
{
	URendererSettings* Settings = GetMutableDefault<URendererSettings>();
	Settings->bEnableRayTracingShadows=bOn;
	Settings->SaveConfig();
}
