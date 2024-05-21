// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/QuasoSnake.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "AI/DC_AIController.h"
#include "BuffSystem/DebuffDisableMovement.h"
#include "BuffSystem/DebuffBlockInputs.h"

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

}

void AQuasoSnake::AttackPlayer(APlayerCharacter* TargetPlayer)
{
	if(!HasAuthority() || bInAttack)
		return;

	Super::AttackPlayer(TargetPlayer);
	
	bInAttack = true;

	FTimerHandle handle;
	FTimerDelegate delegate = FTimerDelegate::CreateUObject(this, &AQuasoSnake::LaunchAtActor, Cast<AActor>(TargetPlayer));
	GetWorld()->GetTimerManager().SetTimer(handle, delegate, WindUpSeconds, false);

	Cast<ADC_AIController>(GetController())->GetBlackboardComponent()->SetValueAsBool("AttackingPlayer", true);

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

		FVector velocity = direction * 50;

		if (distance > 1.f)
			velocity *= distance;
		
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

	FVector midwayPoint = (start + target) * 0.5;
	midwayPoint += FVector::UpVector * 0.15f * (target - start).Length();

	FVector direction = target - start;
	float distance = direction.Length();
	direction.Normalize();

	FVector rightVector = direction.Cross(FVector::DownVector);
	rightVector.Normalize();

	FVector dropDirection = rightVector.Cross(direction);

	dropDirection.Normalize();
	dropDirection = 2*dropDirection + direction;
	dropDirection.Normalize();

	FVector end = target + dropDirection * distance * 0.75;

	TArray<FVector> points = { start, midwayPoint, target, end};

	AttackSpline->SetSplinePoints(points, ESplineCoordinateSpace::World, true);
}

void AQuasoSnake::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(!bLaunched)
		return;

	bLaunched = false;
	AttackTime = 0.f;

	Cast<ADC_AIController>(GetController())->GetBlackboardComponent()->SetValueAsBool("AttackingPlayer", false);
	Cast<ADC_AIController>(GetController())->GetBlackboardComponent()->SetValueAsBool("InAttackRange", false);

	GetWorld()->GetTimerManager().ClearTimer(LaunchHandle);

	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);

	if (!character)
	{
		bInAttack = false;
		return;
	}

	DetachFromControllerPendingDestroy();
	GetCharacterMovement()->DisableMovement();
	Multicast_OnAttachedToPlayer();

	AttachToActor(character, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	PlayerAttachedTo = character;

	SetActorLocation(character->GetActorLocation() + FVector::UpVector * 50);

	GetWorld()->GetTimerManager().SetTimer(StageProgressHandle, this, &AQuasoSnake::ProgressStage, DeathSeconds/3, true, 0.f);

}

void AQuasoSnake::Multicast_OnAttachedToPlayer_Implementation()
{
	GetCapsuleComponent()->SetCollisionProfileName("OverlapAll", true);
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
			PlayerAttachedTo->AddBuffOrDebuff(UDebuffDisableMovement::StaticClass());
			break;

		case 1:
			PlayerAttachedTo->AddBuffOrDebuff(UDebuffBlockInputs::StaticClass());
			break;

		case 2:
			//mute voicechat and blur + dark screen
			break;

		case 3:
			PlayerAttachedTo->TakeDamage(100000.f);
			break;

		default:
			break;

	}
}

void AQuasoSnake::ResetPlayerEffects()
{
	if(!IsValid(PlayerAttachedTo))
		return;

	PlayerAttachedTo->GetCharacterMovement()->SetMovementMode(MOVE_Walking);
	PlayerAttachedTo->GetController()->SetIgnoreLookInput(false);

}
