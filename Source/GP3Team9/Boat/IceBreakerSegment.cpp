

#include "IceBreakerSegment.h"

#include "BoatPawn.h"
#include "IceBreakerComp.h"

void AIceBreakerSegment::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) { return; }
	if (bUsed) { return; }
	auto boat = Cast<ABoatPawn>(OtherActor);
	if (!boat) { return; }
	if (!Cast<APlayerController>(boat->GetController())) { return; }
	bUsed = true;
	boat->iceBreakerComp->IncreaseSegments(1);
	Destroy();
}
