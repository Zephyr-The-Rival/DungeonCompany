
#include "Entities/FootstepSystemComponent.h"

#include "DC_Statics.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter/PlayerCharacter.h"


UFootstepSystemComponent::UFootstepSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UFootstepSystemComponent::BeginPlay()
{
	Super::BeginPlay();
	
	
}


void UFootstepSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//PlayStepsFeedback(); //for test
		
	}
FHitResult UFootstepSystemComponent::GetFootHitResult(FName SocketName) const
{
	FHitResult HitResult;
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Start - FVector(0,0,StepsLineTraceLength);

	if(APlayerCharacter* player = Cast<APlayerCharacter>(GetOwner()))
	{
		Start= player->GetMesh()->GetSocketLocation(SocketName);
		End= Start - FVector(0,0,15);
	}
	 
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.bReturnPhysicalMaterial = true;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 1, 0, 1);

	if (bHit)
		return HitResult;
	else
		return FHitResult();
}

void UFootstepSystemComponent::PlayStepsFeedback(FName SocketName)
{
	
	 FHitResult result= GetFootHitResult(SocketName);

	if(!result.bBlockingHit)
		return;
	



	USoundBase* SoundToPlay;
	UNiagaraSystem* VfxToPlay;
	UPhysicalMaterial* PhysicalMaterial = result.PhysMaterial.Get();
	
 	if(FeedbackMap.Contains(PhysicalMaterial))
 	{
 		 SoundToPlay = FeedbackMap.Find(PhysicalMaterial)->Sound;
 		 VfxToPlay = FeedbackMap.Find(PhysicalMaterial)->VFX;
	}
	else
	{
		 SoundToPlay = DefaultFeedback.Sound;
		 VfxToPlay = DefaultFeedback.VFX;
	}
	


		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),VfxToPlay , result.Location);	
	

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), SoundToPlay, result.Location);
	
	
	 if (APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner()))
	 {
		 playerCharacter->ReportNoise((SoundToPlay)->GetVolumeMultiplier());
	 }
}
