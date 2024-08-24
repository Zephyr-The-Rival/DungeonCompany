// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/CatBurglar/BTService_CatBurglar_SetEyesGlow.h"

#include "AI/DC_AIController.h"
#include "Entities/CatBurglar.h"

UBTService_CatBurglar_SetEyesGlow::UBTService_CatBurglar_SetEyesGlow()
{
	bNotifyBecomeRelevant = true;
	NodeName = "Set Eyes Glow";
}

void UBTService_CatBurglar_SetEyesGlow::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ADC_AIController* aiController = Cast<ADC_AIController>(OwnerComp.GetAIOwner());
	if (!aiController)
		return;

	ACatBurglar* catBurglar = aiController->GetPawn<ACatBurglar>();

	if (!catBurglar)
		return;

	catBurglar->SetAreEyesGlowing(bNewEyesGlow);
}
