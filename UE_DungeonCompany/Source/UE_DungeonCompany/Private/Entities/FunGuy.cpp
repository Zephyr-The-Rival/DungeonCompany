// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/FunGuy.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "AI/DC_AIController.h"
#include "BuffSystem/DebuffPoisonGas.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AISense_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NiagaraComponent.h"

AFunGuy::AFunGuy()
{
	CloudMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CloudMesh"));
	CloudMesh->SetupAttachment(RootComponent);

	CloudSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CloudSphere"));
	CloudSphere->SetupAttachment(RootComponent);

	CloudNiagara = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CloudNiagara"));
	CloudNiagara->SetupAttachment(RootComponent);

	CloudMesh->SetCollisionProfileName("NoCollision");
	CloudSphere->SetCollisionProfileName("AOECollision");

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCharacterMovement()->MaxFlySpeed = 50.f;

	PoisonGasDebuffClass = UDebuffPoisonGas::StaticClass();
}

void AFunGuy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	CalculateCloudStart();
}

void AFunGuy::BeginPlay()
{
	Super::BeginPlay();

	auto material = GetMesh()->GetMaterial(0);

	DynamicMaterial = UMaterialInstanceDynamic::Create(material, this);
	GetMesh()->SetMaterial(0, DynamicMaterial);
	DynamicMaterial->SetScalarParameterValue(FName("PulseFrequency"), PulseFrequency);

	CloudSizeMultiplierPerUpdate = 1 + CloudSizeFactor / 1000;
	
	GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &AFunGuy::UpdateCloud, CloudUpdateInterval, true);
	CloudNiagara->SetFloatParameter("SpawnRadius", 100.0f);

	if (!HasAuthority())
		return;

	CloudSphere->OnComponentBeginOverlap.AddDynamic(this, &AFunGuy::OnCloudBeginOverlap);
	CloudSphere->OnComponentEndOverlap.AddDynamic(this, &AFunGuy::OnCloudEndOverlap);

}

void AFunGuy::CalculateCloudStart()
{
	if (AgeSeconds > MaxSizeAgeSeconds)
		AgeSeconds = MaxSizeAgeSeconds;

	FVector newScale = FVector(1, 1, 1);
	newScale += newScale * AgeSeconds * AgeBonusScaleMultiplier;

	GetCapsuleComponent()->SetRelativeScale3D(newScale);

	int cloudUpscaleNum = AgeSeconds / CloudUpdateInterval;
	CloudSizeMultiplierPerUpdate = 1 + CloudSizeFactor / 1000;

	float newRadius = StartCloudRadius * FMath::Pow(CloudSizeMultiplierPerUpdate, cloudUpscaleNum);
	CloudSphere->SetSphereRadius(newRadius);

	CloudMesh->SetRelativeScale3D(FVector(2, 2, 2) * (newRadius / 100));

	#if WITH_EDITOR
	float maxUpscaleNum = MaxSizeAgeSeconds / CloudUpdateInterval;

	float maxScaleMultiplier = 1 + MaxSizeAgeSeconds * AgeBonusScaleMultiplier;
	float maxRadius = StartCloudRadius * FMath::Pow(CloudSizeMultiplierPerUpdate, maxUpscaleNum);

	FlushPersistentDebugLines(GetWorld());
	DrawDebugSphere(GetWorld(), CloudSphere->GetComponentLocation(), maxRadius * maxScaleMultiplier, 32, FColor::Blue, true);
	#endif
}

void AFunGuy::UpdateCloud()
{
	FVector newScale = CloudMesh->GetRelativeScale3D() * CloudSizeMultiplierPerUpdate;
	CloudMesh->SetRelativeScale3D(newScale);

	if (LastNiagaraScaleUpdate + 0.5f <= newScale.X)
	{
		LastNiagaraScaleUpdate = newScale.X;

		CloudNiagara->SetFloatParameter("SpawnRate", 1.0f);

		FTimerHandle resetHandle;

		FTimerDelegate resetDelegate = FTimerDelegate::CreateLambda([this]() {
			CloudNiagara->SetFloatParameter("SpawnRate", 0.0f);
		});

		GetWorld()->GetTimerManager().SetTimer(resetHandle, resetDelegate, 7.0f, false);
	}

	if (HasAuthority())
		CloudSphere->SetSphereRadius(newScale.X*50);

}

void AFunGuy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(HasAuthority() || AgeSeconds < MaxSizeAgeSeconds)
		AgeSeconds += DeltaSeconds * AgingMultiplier;

	if (AgeSeconds > MaxSizeAgeSeconds)
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
	}
	else
	{
		FVector newScale = FVector(1, 1, 1);
		newScale += newScale * AgeSeconds * AgeBonusScaleMultiplier;
		GetCapsuleComponent()->SetRelativeScale3D(newScale);
	}

	if (!HasAuthority())
		return;

	if (bLifted)
	{
		AddMovementInput(FVector::UpVector, FMath::Sin(AgeSeconds) * WobblingScale);
		return;
	}

	if (AgeSeconds < LiftoffAge)
		return;

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Flying);

	bLifted = true;
	Cast<ADC_AIController>(GetController())->GetBlackboardComponent()->SetValueAsVector(FName("MoveLocation"), GetActorLocation() + FVector::UpVector * LiftoffHeight);

}

void AFunGuy::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFunGuy, AgeSeconds);
}

void AFunGuy::OnCloudBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);

	if (!character)
		return;

	if(PlayerTimerHandles.Contains(character) && PlayerCloudOverlapCount.Contains(character))
	{
		++PlayerCloudOverlapCount[character];
		return;
	}

	PlayerTimerHandles.Add(character);
	PlayerCloudOverlapCount.Add(character, 1);
	
	FTimerDelegate timerDelegate = FTimerDelegate::CreateUObject(this, &AFunGuy::OnSafeTimerElapsed, character);
	GetWorld()->GetTimerManager().SetTimer(PlayerTimerHandles[character], timerDelegate, SafeTime, false);
}

void AFunGuy::OnCloudEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);

	if (!character || !PlayerTimerHandles.Contains(character) || !PlayerCloudOverlapCount.Contains(character))
		return;

	--PlayerCloudOverlapCount[character];
	
	if(!PlayerCloudOverlapCount[character])
	{
		character->RemoveBuffOrDebuff(PoisonGasDebuffClass);
		GetWorld()->GetTimerManager().ClearTimer(PlayerTimerHandles[character]);
		PlayerTimerHandles.Remove(character);
	}
}

void AFunGuy::OnSafeTimerElapsed(APlayerCharacter* PlayerCharacter) const
{
	PlayerCharacter->AddBuffOrDebuff(PoisonGasDebuffClass);
}

TMap<APlayerCharacter*, FTimerHandle> AFunGuy::PlayerTimerHandles;
TMap<APlayerCharacter*, uint16> AFunGuy::PlayerCloudOverlapCount;
