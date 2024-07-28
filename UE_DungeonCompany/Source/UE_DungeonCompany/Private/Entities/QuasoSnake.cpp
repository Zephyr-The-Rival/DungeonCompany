// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/QuasoSnake.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "AI/DC_AIController.h"
#include "BuffSystem/DebuffDisableMovement.h"
#include "BuffSystem/DebuffBlockInputs.h"
#include "BuffSystem/DebuffMuffledVoice.h"
#include "BuffSystem/DebuffImpairedVision.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SplineComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h" 

AQuasoSnake::AQuasoSnake()
{
	AttackSpline = CreateDefaultSubobject<USplineComponent>("AttackSpline");

	EyeCollision = CreateDefaultSubobject<USphereComponent>("EyeCollision");
	EyeCollision->SetupAttachment(RootComponent);
}

void AQuasoSnake::BeginPlay()
{
	Super::BeginPlay();

	AttackSpline->Duration = 0.75f;

	if(!HasAuthority())
		return;

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AQuasoSnake::OnHit);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);

	GetCharacterMovement()->DisableMovement();

}

void AQuasoSnake::AttackPlayer(APlayerCharacter* TargetPlayer)
{
	if(!HasAuthority() || bInAttack)
		return;

	bInAttack = true;

	FTimerHandle handle;
	FTimerDelegate delegate = FTimerDelegate::CreateUObject(this, &AQuasoSnake::LaunchAtActor, Cast<AActor>(TargetPlayer));
	GetWorld()->GetTimerManager().SetTimer(handle, delegate, WindUpSeconds, false);

	SetInAttackOnBlackboard(true);

	GetCharacterMovement()->Velocity = FVector(0, 0, 0);

	GetCharacterMovement()->SetMovementMode(MOVE_None);

	FRotator attackRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), TargetPlayer->GetActorLocation());
	GetController()->SetControlRotation(attackRotation);

	CalculateLaunchSplineToActor(TargetPlayer);

}

void AQuasoSnake::LaunchAtActor(AActor* Actor)
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	FTimerDelegate delegate = FTimerDelegate::CreateLambda([this]() {

		AttackTime += LaunchUpdateInterval;

		if (AttackTime > AttackSpline->Duration)
		{
			GetWorld()->GetTimerManager().ClearTimer(LaunchHandle);
			return;
		}
		
		FVector splineLocation = AttackSpline->GetLocationAtTime(AttackTime, ESplineCoordinateSpace::World);

		FVector direction = splineLocation - GetActorLocation();
		float distance = direction.Length();
		direction.Normalize();

		FVector velocity = direction * 50 * distance;
		
		GetCharacterMovement()->Velocity = velocity;

		AddMovementInput(velocity);
	});

	bLaunched = true;

	Jump();
	GetWorld()->GetTimerManager().SetTimer(LaunchHandle, delegate, LaunchUpdateInterval, true);

}

void AQuasoSnake::CalculateLaunchSplineToActor(AActor* Actor)
{
	FVector start = GetActorLocation();
	FVector target = Actor->GetActorLocation() + FVector::UpVector * 75;

	FVector jumpVector = target - start;

	FVector midwayPoint = (start + target) * 0.5;
	midwayPoint += FVector::UpVector * 0.15f * jumpVector.Length();
	if(jumpVector.Z > 0.f)
		jumpVector.Z = -jumpVector.Z;

	FVector end = target + jumpVector;

	FVector secMidwayPoint = (target + end) * 0.5;
	secMidwayPoint += FVector::UpVector * 0.15f * jumpVector.Length();

	TArray<FVector> points = { start, midwayPoint, target, secMidwayPoint, end};

	AttackSpline->SetSplinePoints(points, ESplineCoordinateSpace::World, true);
}

void AQuasoSnake::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!bLaunched)
		return;

	bLaunched = false;
	AttackTime = 0.f;

	ADC_AIController* aiController = GetController<ADC_AIController>();
	if (!aiController)
		return;

	aiController->GetBlackboardComponent()->SetValueAsBool("InAttackRange", false);

	GetWorld()->GetTimerManager().ClearTimer(LaunchHandle);

	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);

	if (!character)
	{
		bInAttack = false;
		aiController->GetBlackboardComponent()->SetValueAsBool("AttackingPlayer", false);
		return;
	}
	
	GetCharacterMovement()->DisableMovement();
	Multicast_OnAttachedToPlayer();

	AttachToActor(character, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	PlayerAttachedTo = character;

	SetActorLocation(character->GetActorLocation() + FVector::UpVector * 100);

	GetWorld()->GetTimerManager().SetTimer(StageProgressHandle, this, &AQuasoSnake::ProgressStage, DeathSeconds/3, true, 0.f);

}

void AQuasoSnake::Multicast_OnAttachedToPlayer_Implementation()
{
	GetCapsuleComponent()->SetCollisionProfileName("OverlapAll", true);
}

void AQuasoSnake::Multicast_OnDetachedFromPlayer_Implementation()
{
	GetCapsuleComponent()->SetCollisionProfileName("Pawn", true);
}

void AQuasoSnake::ReturnToVolume()
{
	OnWantsToReturnToVolume.Broadcast();

	Destroy();
}

void AQuasoSnake::OnDeath_Implementation()
{
	Super::OnDeath_Implementation();

	if(!HasAuthority())
		return;

	ResetPlayerEffects();

}

void AQuasoSnake::ProgressStage()
{
	if(!IsValid(PlayerAttachedTo))
		return;

	++CurrentStage;

	switch (CurrentStage)
	{
		case 0:
			AppliedDebuffs.Add(PlayerAttachedTo->AddBuffOrDebuff(this->DisableMovementDebuff));
			break;

		case 1:
			AppliedDebuffs.Add(PlayerAttachedTo->AddBuffOrDebuff(this->BlockInputsDebuff));
			break;

		case 2:
			AppliedDebuffs.Add(PlayerAttachedTo->AddBuffOrDebuff(this->MuffledVoiceDebuff));
			AppliedDebuffs.Add(PlayerAttachedTo->AddBuffOrDebuff(this->ImpairedVisionDebuff));
			break;

		case 3:
			PlayerAttachedTo->TakeDamage(100000.f);
			DetachFromPlayer();
			CurrentStage = -1;
			break;

		default:
			break;
	}
}

void AQuasoSnake::ResetPlayerEffects()
{
	if(!IsValid(PlayerAttachedTo))
		return;

	int debuffsNum = AppliedDebuffs.Num();

	for (int i = 0; i < debuffsNum; ++i)
	{
		if(!IsValid(AppliedDebuffs[i]))
			continue;

		AppliedDebuffs[i]->Destroy();
	}
}

void AQuasoSnake::DetachFromPlayer()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Multicast_OnDetachedFromPlayer();

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	bInAttack = false;

	ADC_AIController* aiController = Cast<ADC_AIController>(GetController());
	if (!aiController)
		return;

	aiController->GetBlackboardComponent()->SetValueAsBool("AttackingPlayer", false);
	aiController->GetBlackboardComponent()->ClearValue("TargetPlayer");
}
