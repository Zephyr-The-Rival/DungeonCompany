// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Common/CommonValueChanger.h"

void UCommonValueChanger::OnValueChanged(float NewValue)
{
	OnValueChangedEvent.Broadcast(NewValue);
}
