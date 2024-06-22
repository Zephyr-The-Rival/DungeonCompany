// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ClimbingHook/RopeEndComponent.h"

URopeEndComponent::URopeEndComponent()
{
	bInteractOnServer = true;
	//SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//SetSimulatePhysics(true);
}

void URopeEndComponent::AuthorityInteract(APawn* InteractingPawn)
{
	
}

void URopeEndComponent::Throw(FVector Direction, float Strength)
{
	AddForce(Direction*Strength);
}
