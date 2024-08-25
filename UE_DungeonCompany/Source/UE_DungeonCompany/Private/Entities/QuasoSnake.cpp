// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/QuasoSnake.h"

#include "NiagaraFunctionLibrary.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "AI/DC_AIController.h"
#include "Animation/AnimSingleNodeInstance.h"
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

	TopCaveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TopCaveMesh"));
	TopCaveMesh->SetupAttachment(GetMesh(), FName("TopCaveSocket"));

	BottomCaveMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BottomCaveMesh"));
	BottomCaveMesh->SetupAttachment(GetMesh(), FName("BottomCaveSocket"));

	MaxHP = 50.f;
}

void AQuasoSnake::BeginPlay()
{
	Super::BeginPlay();

	AttackSpline->Duration = 0.75f;

	if (!HasAuthority())
		return;

	GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &AQuasoSnake::OnHit);
	GetCapsuleComponent()->SetNotifyRigidBodyCollision(true);

	GetCharacterMovement()->DisableMovement();

	SetIsLurking(true);
}

void AQuasoSnake::OnAnimationFlagUpdated_Implementation()
{
	Super::OnAnimationFlagUpdated_Implementation();

	TopCaveMesh->SetVisibility(IsLurking());
	BottomCaveMesh->SetVisibility(IsLurking());
}

void AQuasoSnake::StopLurking()
{
	SetIsLurking(false);

	if(!JumpOutOfWallEffect)
		return;
	
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), JumpOutOfWallEffect, GetActorLocation(), GetActorRotation());
}

void AQuasoSnake::AttackPlayer(APlayerCharacter* PlayerAttacking)
{
	if (!HasAuthority() || bInAttack)
		return;

	bInAttack = true;
	SetIsAttacking(true);

	FTimerHandle handle;
	FTimerDelegate delegate = FTimerDelegate::CreateUObject(this, &AQuasoSnake::LaunchAtActor,
	                                                        Cast<AActor>(PlayerAttacking));
	GetWorld()->GetTimerManager().SetTimer(handle, delegate, WindUpSeconds, false);

	SetInAttackOnBlackboard(true);

	GetCharacterMovement()->Velocity = FVector(0, 0, 0);

	GetCharacterMovement()->SetMovementMode(MOVE_None);

	FRotator attackRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(),
	                                                                 PlayerAttacking->GetActorLocation());
	GetController()->SetControlRotation(attackRotation);

	CalculateLaunchSplineToActor(PlayerAttacking);
}

void AQuasoSnake::LaunchAtActor(AActor* Actor)
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	FTimerDelegate delegate = FTimerDelegate::CreateLambda([this]()
	{
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
	if (jumpVector.Z > 0.f)
		jumpVector.Z = -jumpVector.Z;

	FVector end = target + jumpVector;

	FVector secMidwayPoint = (target + end) * 0.5;
	secMidwayPoint += FVector::UpVector * 0.15f * jumpVector.Length();

	TArray<FVector> points = {start, midwayPoint, target, secMidwayPoint, end};

	AttackSpline->SetSplinePoints(points, ESplineCoordinateSpace::World, true);
}

void AQuasoSnake::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                        FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bLaunched)
		return;

	bLaunched = false;
	AttackTime = 0.f;

	SetIsAttacking(false);

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
	Multicast_OnAttachedToPlayer(character);

	AttachToComponent(character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale,
	                  TEXT("QuasoSocket"));
	PlayerAttachedTo = character;

	SetIsAttachedToPlayer(true);

	GetWorld()->GetTimerManager().SetTimer(StageProgressHandle, this, &AQuasoSnake::ProgressStage, DeathSeconds / 3,
	                                       true, 0.f);
}

void AQuasoSnake::Multicast_OnAttachedToPlayer_Implementation(APlayerCharacter* AttachedPlayer)
{
	GetCapsuleComponent()->SetCollisionProfileName("OverlapAll", true);

	if (!IsValid(AttachedPlayer) || !AttachedPlayer->IsLocallyControlled())
		return;

	GetMesh()->SetVisibility(false);
	FirstPersonAttach = NewObject<USkeletalMeshComponent>(AttachedPlayer);
	FirstPersonAttach->SetSkeletalMesh(GetMesh()->GetSkeletalMeshAsset());
	FirstPersonAttach->SetupAttachment(AttachedPlayer->GetFirstPersonMesh(), TEXT("QuasoSocket"));

	FirstPersonAttach->RegisterComponent();
	FirstPersonAttach->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	FirstPersonAttach->SetAnimation(FirstPersonChoke);
}

void AQuasoSnake::Multicast_OnDetachedFromPlayer_Implementation()
{
	GetCapsuleComponent()->SetCollisionProfileName("Pawn", true);

	if (!IsValid(FirstPersonAttach))
		return;

	FirstPersonAttach->DestroyComponent();
}

void AQuasoSnake::ReturnToVolume()
{
	OnWantsToReturnToVolume.Broadcast();

	Destroy();
}

void AQuasoSnake::SetIsAttachedToPlayer(bool InIsAttached)
{
	if (InIsAttached == IsAttachedToPlayer())
		return;

	ToggleAnimationBitFlag(AAIEntity::FLAG_Custom_0);
}

void AQuasoSnake::SetIsLurking(bool InIsLurking)
{
	if (InIsLurking == IsLurking())
		return;

	ToggleAnimationBitFlag(AAIEntity::FLAG_Custom_1);
}

void AQuasoSnake::OnDeath_Implementation()
{
	Super::OnDeath_Implementation();

	if (!HasAuthority())
		return;

	GetWorld()->GetTimerManager().ClearTimer(StageProgressHandle);
	
	ResetPlayerEffects();
	DetachFromPlayer();
	
}

void AQuasoSnake::ProgressStage()
{
	if (!IsValid(PlayerAttachedTo))
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
	if (!IsValid(PlayerAttachedTo))
		return;

	int debuffsNum = AppliedDebuffs.Num();

	for (int i = 0; i < debuffsNum; ++i)
	{
		if (!IsValid(AppliedDebuffs[i]))
			continue;

		AppliedDebuffs[i]->Destroy();
	}
}

void AQuasoSnake::DetachFromPlayer()
{
	if (!IsAttachedToPlayer())
		return;

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Multicast_OnDetachedFromPlayer();

	GetCharacterMovement()->SetMovementMode(MOVE_Walking);

	bInAttack = false;

	SetIsAttachedToPlayer(false);

	ADC_AIController* aiController = Cast<ADC_AIController>(GetController());
	if (!aiController)
		return;

	aiController->GetBlackboardComponent()->SetValueAsBool("AttackingPlayer", false);
	aiController->GetBlackboardComponent()->ClearValue("TargetPlayer");
}
