

#include "IceBreakerComp.h"

#include "BoatPawn.h"

#include "Components/StaticMeshComponent.h"


UIceBreakerComp::UIceBreakerComp()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UIceBreakerComp::IsIceBreakerComplete()
{
	return currentSegments >= segmentsRequierd;
}

void UIceBreakerComp::IncreaseSegments(int amountToAdd)
{
	currentSegments = FMath::Clamp(currentSegments + amountToAdd, 0, segmentsRequierd);
	if (IsIceBreakerComplete())
	{
		if (icebreakerMesh) 
		{
			icebreakerMesh->SetHiddenInGame(false);
		}
	}
}

int UIceBreakerComp::GetSegementCount()
{
	return currentSegments;
}

void UIceBreakerComp::Initalize()
{
	auto owner = GetOwner();
	if (!ensure(owner)) { return; }
	auto boat = Cast<ABoatPawn>(owner);
	if (!ensure(boat)) { return; }

	icebreakerMesh = boat->IceBreakerMesh;
	if (!icebreakerMesh) { return; }
	icebreakerMesh->SetHiddenInGame(!bStartWithIceBreaker);
}


void UIceBreakerComp::BeginPlay()
{
	Super::BeginPlay();
}