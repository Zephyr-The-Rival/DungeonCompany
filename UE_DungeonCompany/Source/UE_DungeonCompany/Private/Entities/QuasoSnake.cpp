// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/QuasoSnake.h"

#include "PlayerCharacter/PlayerCharacter.h"
#include "AI/DC_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SplineComponent.h"

AQuasoSnake::AQuasoSnake()
{
	AttackSpline = CreateDefaultSubobject<USplineComponent>("AttackSpline");
}

void AQuasoSnake::BeginPlay()
{
	Super::BeginPlay();

	AttackSpline->Duration = 0.5f;

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
	GetWorld()->GetTimerManager().SetTimer(handle, delegate, WindUpTime, false);

	Cast<ADC_AIController>(GetController())->GetBlackboardComponent()->SetValueAsBool("AttackingPlayer", true);

	GetCharacterMovement()->Velocity = FVector(0, 0, 0);

	GetCharacterMovement()->SetMovementMode(MOVE_None);

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

		GetCharacterMovement()->Velocity = direction * 50 * distance;

		AddMovementInput(GetCharacterMovement()->Velocity);

		UE_LOG(LogTemp, Warning, TEXT("%d"), GetCharacterMovement()->MovementMode);
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

	TArray<FVector> points = { start, midwayPoint, target};

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

	Multicast_OnAttachedToPlayer();
	AttachToActor(character, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	SetActorLocation(character->GetActorLocation() + FVector::UpVector * 100);
	Cast<ADC_AIController>(GetController())->GetBlackboardComponent()->SetValueAsObject("PlayerAttachedTo", character);

}


void AQuasoSnake::Multicast_OnAttachedToPlayer_Implementation()
{
	GetCapsuleComponent()->SetCollisionProfileName("OverlapAll", true);
	GetCharacterMovement()->Deactivate();
}