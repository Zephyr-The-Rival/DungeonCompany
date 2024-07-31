// Fill out your copyright notice in the Description page of Project Settings.


#include "BuffSystem/BuffDebuffBase.h"
#include "Entities/DC_Entity.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "UI/PlayerHud/PlayerHud.h"

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

	if (OuterEntity->HasAuthority())
		AuthorityRemove();

}

void UBuffDebuffBase::AuthorityApply()
{
	Timegate(ActiveSeconds);

}

void UBuffDebuffBase::LocalApply()
{
	if (APlayerCharacter* player = Cast<APlayerCharacter>(OuterEntity))
		player->GetMyHud()->UdateBuffs();
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

	if(OuterEntity->HasAuthority())
		AuthorityRemove();
}

void UBuffDebuffBase::AuthorityRemove()
{

}

void UBuffDebuffBase::LocalRemove()
{
	if (APlayerCharacter* player = Cast<APlayerCharacter>(OuterEntity))
		player->GetMyHud()->UdateBuffs();
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
