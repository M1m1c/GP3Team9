
#include "FloatMasterComp.h"

#include "FloatComp.h"

#include "GP3Team9/Nelson/WaveHeightmap.h"

UFloatMasterComp::UFloatMasterComp()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFloatMasterComp::BeginPlay()
{
	Super::BeginPlay();

	auto owner = GetOwner();

	auto comps = owner->GetComponentsByClass(UFloatComp::StaticClass());

	for (auto item : comps)
	{
		floatingComps.Add(Cast<UFloatComp>(item));
	}

	for (auto item : floatingComps)
	{
		item->Initalize(waveHeightMapAsset, floatingComps.Num());
	}
	
}

