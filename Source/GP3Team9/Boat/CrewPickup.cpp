

#include "CrewPickup.h"

#include "BoatPawn.h"
#include "FloatMasterComp.h"
#include "CrewComp.h"

#include "Components/SphereComponent.h"

ACrewPickup::ACrewPickup()
{

}

void ACrewPickup::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor) { return; }
	if (bUsed) { return; }
	auto boat = Cast<ABoatPawn>(OtherActor);
	if (!boat) { return; }
	if (!Cast<APlayerController>(boat->GetController())) { return; }
	bUsed = true;
	boat->crewComp->AddNewCrew(crewToAdd);
	Destroy();
}

