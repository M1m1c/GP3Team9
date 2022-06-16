

#include "DestructibleIce.h"
#include "DestructibleComponent.h"

#include "DesktopPlatform/Private/DesktopPlatformPrivate.h"


ADestructibleIce::ADestructibleIce()
{
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Component"));

	DestructibleComponent = CreateDefaultSubobject<UDestructibleComponent>(TEXT("Destructible Component"));
	DestructibleComponent->SetupAttachment(RootComponent);
	DestructibleComponent->SetNotifyRigidBodyCollision(true);

	TriggerComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger Component"));
	TriggerComponent->SetupAttachment(RootComponent);
}

void ADestructibleIce::Damage(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
}

void ADestructibleIce::Trigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag("Player"))
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Player sweep")));
		
		DestructibleComponent->ApplyDamage(1000, SweepResult.Location, SweepResult.Normal * -1, 1000);
		DestructibleComponent->ApplyRadiusDamage(1000, SweepResult.Location, 100.f, 1000, true);
		
	}
}

void ADestructibleIce::Destroy()
{
}

void ADestructibleIce::BeginPlay()
{
	Super::BeginPlay();
	
	DestructibleComponent->OnComponentHit.AddDynamic(this, &ADestructibleIce::Damage);
	
	TriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ADestructibleIce::Trigger);
	
}

void ADestructibleIce::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

