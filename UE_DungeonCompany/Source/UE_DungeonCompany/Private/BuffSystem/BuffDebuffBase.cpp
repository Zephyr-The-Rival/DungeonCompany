// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSystem/BuffDebuffBase.h"
#include "Entities/DC_Entity.h"

UBuffDebuffBase::UBuffDebuffBase()
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);

}


void UBuffDebuffBase::BeginPlay()
{
	Super::BeginPlay();

	OuterEntity = Cast<ADC_Entity>(GetOuter());

	if (!OuterEntity)
	{
		DestroyComponent(false);
		return;
	}

	Apply();

}

void UBuffDebuffBase::Apply()
{
	if (OuterEntity->IsLocallyControlled())
		LocalApply();

	Timegate(ActiveSeconds);
}

void UBuffDebuffBase::LocalApply()
{

}

void UBuffDebuffBase::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if(!IsValid(OuterEntity))
		return;

	Remove();

}

void UBuffDebuffBase::Remove()
{
	if (OuterEntity->IsLocallyControlled())
		LocalRemove();
}

void UBuffDebuffBase::LocalRemove()
{

}

void UBuffDebuffBase::Destroy()
{
	GetWorld()->GetTimerManager().ClearTimer(ActiveTimerHandle);

	Super::DestroyComponent(false);
}

void UBuffDebuffBase::Timegate(float TimeSeconds)
{
	if(TimeSeconds > 0.f)
		GetWorld()->GetTimerManager().SetTimer(ActiveTimerHandle, this, &UBuffDebuffBase::Destroy, TimeSeconds, false);
}
