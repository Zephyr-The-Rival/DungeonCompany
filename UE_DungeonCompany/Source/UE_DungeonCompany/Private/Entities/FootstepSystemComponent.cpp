
#include "Entities/FootstepSystemComponent.h"

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
UPhysicalMaterial* UFootstepSystemComponent::GetFootMaterial() const
{
	FHitResult HitResult;
	FVector Start = GetOwner()->GetActorLocation();
	FVector End = Start - FVector(0,0,StepsLineTraceLength);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());
	Params.bReturnPhysicalMaterial = true;

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);
	DrawDebugLine(GetWorld(), Start, End, bHit ? FColor::Green : FColor::Red, false, 1, 0, 1);

	if (bHit && HitResult.PhysMaterial.IsValid())
	{
		return HitResult.PhysMaterial.Get();
	}
	return nullptr;
}

void UFootstepSystemComponent::PlayStepsFeedback()
{
	if (const UPhysicalMaterial* PhysicalMaterial = GetFootMaterial())
	{
		if (UNiagaraSystem** FoundVfx = SurfaceVFX.Find(PhysicalMaterial); FoundVfx && *FoundVfx)
		{
			const FVector Location = GetOwner()->GetActorLocation();
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),*FoundVfx,Location);
		}

		if (USoundBase** FoundSfx = SurfaceSFX.Find(PhysicalMaterial); FoundSfx && *FoundSfx)
		{
			const FVector Location = GetOwner()->GetActorLocation(); 
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), *FoundSfx, Location);
			
			if(APlayerCharacter* playerCharacter = Cast<APlayerCharacter>(GetOwner()))
			{
				playerCharacter->ReportNoise((*FoundSfx)->GetVolumeMultiplier());
			}
		}
	}
}
