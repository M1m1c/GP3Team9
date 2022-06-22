
#include "FloatDisablerSphere.h"
#include "FloatMasterComp.h"

#include "Components/SphereComponent.h"

AFloatDisablerSphere::AFloatDisablerSphere()
{
	sphere = CreateDefaultSubobject<USphereComponent>(FName("collider"));
	SetRootComponent(sphere);
	sphere->OnComponentBeginOverlap.AddDynamic(this, &AFloatDisablerSphere::OnOverlapBegin);
	sphere->OnComponentEndOverlap.AddDynamic(this, &AFloatDisablerSphere::OnOverlapEnd);
	sphere->bShouldCollideWhenPlacing = true;
}

void AFloatDisablerSphere::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor) { return; }
	if (OtherActor == Owner) { return; }
	auto comp = OtherActor->GetComponentByClass(UFloatMasterComp::StaticClass());
	auto floatMaster = Cast<UFloatMasterComp>(comp);
	if (floatMaster) 
	{
		floatMaster->EnableFloating();
	}
}

void AFloatDisablerSphere::OnOverlapEnd(
	class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp,
	int32 OtherBodyInd)
{
	if (!OtherActor) { return; }
	if (OtherActor == Owner) { return; }
	auto comp = OtherActor->GetComponentByClass(UFloatMasterComp::StaticClass());
	auto floatMaster = Cast<UFloatMasterComp>(comp);
	if (floatMaster)
	{
		floatMaster->DisableFloating();
	}
}

void AFloatDisablerSphere::SetOwner(AActor* newOwner)
{
	if (!newOwner) { return; }
	Owner = newOwner;
}

void AFloatDisablerSphere::SetScale(FVector scaleToSet) 
{

	sizeToSet = scaleToSet;
	GetWorldTimerManager().SetTimer(initialiseSizeTimerHandle, this, &AFloatDisablerSphere::UpdateScale, 0.1f, false, 0.5f);
}

void AFloatDisablerSphere::UpdateScale()
{
	SetActorScale3D(sizeToSet);
	GetWorldTimerManager().ClearTimer(initialiseSizeTimerHandle);
}
