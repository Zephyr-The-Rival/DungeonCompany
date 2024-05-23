// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

class APlayerCharacter;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UInteractable : public UInterface
{
	GENERATED_BODY()

};

/**
 * 
 */
class UE_DUNGEONCOMPANY_API IInteractable
{
	GENERATED_BODY()

protected:
	bool bInteractable = true;
	bool bInteractOnServer = false;

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void Interact(APawn* InteractingPawn)=0;
	virtual void AuthorityInteract(APawn* InteractingPawn){};
	
	inline bool IsInteractable() const { return bInteractable; }
	inline bool IsInteractionRunningOnServer() const { return bInteractOnServer; }
};
