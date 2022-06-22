

#include "FloatingProp.h"
#include "FloatMasterComp.h"
#include "Components/SphereComponent.h"

AFloatingProp::AFloatingProp()
{

	physicsBody = CreateDefaultSubobject<UStaticMeshComponent>(FName("physicsBody"));
	SetRootComponent(physicsBody);

	collider = CreateDefaultSubobject<USphereComponent>(FName("collider"));
	collider->SetupAttachment(physicsBody);
	collider->OnComponentBeginOverlap.AddDynamic(this, &AFloatingProp::OnOverlapBegin);
	collider->OnComponentHit.AddDynamic(this, &AFloatingProp::OnComponentHit);


	floatMasterComp = CreateDefaultSubobject<UFloatMasterComp>(TEXT("FloatMasterComp"));

}

inline void AFloatingProp::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{

}

void AFloatingProp::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

}

UStaticMeshComponent* AFloatingProp::GetFloatBody()
{
	return physicsBody;
}

void AFloatingProp::BeginPlay()
{
	Super::BeginPlay();

	physicsBody->SetSimulatePhysics(true);
	physicsBody->SetEnableGravity(false);
	defaultAngularDamping = physicsBody->GetAngularDamping();
	defaultLinearDamping = physicsBody->GetLinearDamping();

	collider->SetCollisionProfileName(FName("OverlapAll"));
}

void AFloatingProp::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (physicsBody)
	{
		if (previousDeltaTime < 0.f) { previousDeltaTime = DeltaTime; }
		auto fps = previousDeltaTime / 1.f;
		physicsBody->SetLinearDamping(FMath::Clamp(FMath::Lerp(defaultLinearDamping, 100.f, fps), defaultLinearDamping, 100.f));
		physicsBody->SetAngularDamping(FMath::Clamp(FMath::Lerp(defaultAngularDamping, 100.f, fps), defaultLinearDamping, 100.f));
		previousDeltaTime = DeltaTime;
	}
}

void  AFloatingProp::SetMesh(UStaticMesh* meshToSet)
{
	physicsBody->SetStaticMesh(meshToSet);
}


