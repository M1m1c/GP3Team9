

#include "Seamine.h"

#include "BoatPawn.h"
#include "HealthComp.h"

#include "Components/SphereComponent.h"

ASeamine::ASeamine()
{
	OnActorEndOverlap.AddDynamic(this, &ASeamine::OnOverlapExit);
}

void ASeamine::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) { return; }
	auto boat = Cast<ABoatPawn>(OtherActor);
	if (!boat) { return; }

	if (boatsInExplosionRadius.Contains(boat)) { return; }
	boatsInExplosionRadius.Add(boat);

	if (bUsed) { return; }
	bUsed = true;
	OnExplosion();
}

void ASeamine::OnOverlapExit(AActor* self, AActor* OtherActor)
{
	if (!OtherActor) { return; }
	auto boat = Cast<ABoatPawn>(OtherActor);
	if (!boat) { return; }
	if (boatsInExplosionRadius.Contains(boat))
	{
		UE_LOG(LogTemp, Warning, TEXT("boat Removed"));
		boatsInExplosionRadius.Remove(boat);
	}
}

