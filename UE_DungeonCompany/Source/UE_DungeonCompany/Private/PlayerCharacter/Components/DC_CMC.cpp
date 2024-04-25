// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter/Components/DC_CMC.h"

bool UDC_CMC::CanAttemptJump() const
{
	return IsJumpAllowed() &&
		(IsMovingOnGround() || IsFalling());

}
