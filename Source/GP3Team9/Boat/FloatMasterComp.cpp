
#include "FloatMasterComp.h"

#include "FloatComp.h"

#include "GP3Team9/Nelson/WaveHeightmap.h"

#include "GameFramework/Actor.h"

UFloatMasterComp::UFloatMasterComp()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFloatMasterComp::DisableFloating()
{
	//TODO pick two random float points to disable then wait half a second and disable two more,
	//This should rpoduce some coler sinking animations

	auto jumpDeathRand = FMath::RandRange(1, 3);

	if (jumpDeathRand == 3) 
	{
		for (auto floater : floatingComps)
		{
			floater->SetSinkGravity(-980.f, 1.f);
		}
	}
	

	GetOwner()->GetWorldTimerManager().SetTimer(DisableFloatingTimer, this, &UFloatMasterComp::DisableRandomFloatPoints, 0.7f, true, 0.f);
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
		item->Initalize(waveHeightMapAsset, floatingComps.Num(),gravity,defaultGravityAccel,submergedDivider,massMultiplier);
	}

}

void UFloatMasterComp::DisableRandomFloatPoints()
{
	if (disabledFloaters == floatingComps.Num())
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(DisableFloatingTimer);
		return;
	}

	int i = 0;

	while (i < 30)
	{
		i++;
		auto randIndex = FMath::RandRange(0, floatingComps.Num() - 1);
		auto floater = floatingComps[randIndex];
		if (floater->GetIsFloatingActive() == false) { continue; }
		floater->SetSinkGravity(-980.f, 1.f);
		floater->ToggleFloatingActive();
		disabledFloaters++;
		return;
	}
}

