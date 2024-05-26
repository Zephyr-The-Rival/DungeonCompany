// Fill out your copyright notice in the Description page of Project Settings.


#include "Entities/FunGuy.h"
#include "PlayerCharacter/PlayerCharacter.h"
#include "AI/DC_AIController.h"

#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Perception/AISense_Hearing.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
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

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_None);
	GetCharacterMovement()->MaxFlySpeed = 50.f;

}

void AFunGuy::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (AgeSeconds > MaxSizeAgeSeconds)
		AgeSeconds = MaxSizeAgeSeconds;

	FVector newScale = FVector(1, 1, 1);
	newScale += newScale * AgeSeconds * AgeBonusScaleMultiplier;

	GetCapsuleComponent()->SetRelativeScale3D(newScale);

	int cloudUpscaleNum = AgeSeconds / CloudUpdateInterval;
	float newCloudRadius = StartCloudRadius;

	CloudSizeMultiplierPerUpdate = 1 + CloudSizeFactor / 1000;

	for (int i = 0; i < cloudUpscaleNum; ++i)
		newCloudRadius *= CloudSizeMultiplierPerUpdate;
	
	CloudMesh->SetWorldScale3D(FVector(1, 1, 1) * newCloudRadius /50);
	CloudSphere->SetSphereRadius(newCloudRadius);

}

void AFunGuy::BeginPlay()
{
	Super::BeginPlay();

	auto material = GetMesh()->GetMaterial(0);

	DynamicMaterial = UMaterialInstanceDynamic::Create(material, this);
	GetMesh()->SetMaterial(0, DynamicMaterial);
	DynamicMaterial->SetScalarParameterValue(FName("PulseFrequency"), PulseFrequency);

	CloudSizeMultiplierPerUpdate = 1 + CloudSizeFactor / 1000;

	FTimerDelegate updateDelegate = FTimerDelegate::CreateLambda([this]() 
		{
			FVector newScale = CloudMesh->GetRelativeScale3D() * CloudSizeMultiplierPerUpdate;
			CloudMesh->SetRelativeScale3D(newScale);

			if (HasAuthority()) 
			{
				float radius = CloudSphere->GetUnscaledSphereRadius() * CloudSizeMultiplierPerUpdate;
				CloudSphere->SetSphereRadius(radius);
			}
		}
	);
	
	GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, updateDelegate, CloudUpdateInterval, true);

	if (!HasAuthority())
		return;

	CloudSphere->OnComponentBeginOverlap.AddDynamic(this, &AFunGuy::OnCloudBeginOverlap);
	CloudSphere->OnComponentEndOverlap.AddDynamic(this, &AFunGuy::OnCloudEndOverlap);

}

void AFunGuy::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (AgeSeconds >= MaxSizeAgeSeconds)
		return;

	AgeSeconds += DeltaSeconds * AgingMultiplier;

	if (AgeSeconds > MaxSizeAgeSeconds)
	{
		GetWorld()->GetTimerManager().ClearTimer(UpdateTimerHandle);
		AgeSeconds = MaxSizeAgeSeconds;
	}

	FVector newScale = FVector(1, 1, 1);
	newScale += newScale * AgeSeconds * AgeBonusScaleMultiplier;
	GetCapsuleComponent()->SetRelativeScale3D(newScale);

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

	FTimerDelegate timerDelegate = FTimerDelegate::CreateUObject(this, &AFunGuy::OnSafeTimerElapsed, character);

	PlayerTimerHandles.Add(character);

	GetWorld()->GetTimerManager().SetTimer(PlayerTimerHandles[character], timerDelegate, SafeTime, false);


}

void AFunGuy::OnCloudEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	APlayerCharacter* character = Cast<APlayerCharacter>(OtherActor);

	if (!character)
		return;

	GetWorld()->GetTimerManager().ClearTimer(PlayerTimerHandles[character]);
	PlayerTimerHandles.Remove(character);
}

void AFunGuy::OnSafeTimerElapsed(APlayerCharacter* PlayerCharacter)
{
	FTimerDelegate timerDelegate = FTimerDelegate::CreateUObject(this, &AFunGuy::OnDamageTimerElapsed, PlayerCharacter);

	GetWorld()->GetTimerManager().SetTimer(PlayerTimerHandles[PlayerCharacter], timerDelegate, DamageInterval, true, 0.f);

}

void AFunGuy::OnDamageTimerElapsed(APlayerCharacter* PlayerCharacter)
{
	if (!IsValid(PlayerCharacter))
	{
		GetWorld()->GetTimerManager().ClearTimer(PlayerTimerHandles[PlayerCharacter]);
		PlayerTimerHandles.Remove(PlayerCharacter);
		return;
	}

	PlayerCharacter->TakeDamage(Damage);
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), PlayerCharacter->GetActorLocation(), 2.f, PlayerCharacter);

}

void AFunGuy::OnDeath_Implementation()
{
	Super::OnDeath_Implementation();

	Destroy();
}
