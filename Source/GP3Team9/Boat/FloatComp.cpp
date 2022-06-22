

#include "FloatComp.h"
#include "Floatable.h"

#include "GP3Team9/Nelson/WaveHeightmap.h"

UFloatComp::UFloatComp()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;

}

void UFloatComp::Initalize(UWaveHeightmap* waveMap, float compCount, float grav, float gravAccel, float subMulti, float massMulti)
{
	waveHeightMapAsset = waveMap;
	if (!ensure(waveHeightMapAsset))
	{
		this->DestroyComponent();
		return;
	}

	floatingCompCount = compCount;
	gravity = grav;
	defaultGravityAccel = gravAccel;
	submergedDivider = subMulti;
	massMultiplier = massMulti;

	world = GetWorld();
	if (!ensure(world)) { return; }

	auto owner = GetOwner();
	if (!ensure(owner)) { return; }

	auto floatable = Cast<IFloatable>(owner);
	if (!ensure(floatable)) { return; }

	floatBody = floatable->GetFloatBody();
	if (!ensure(floatBody)) { return; }


	initalized = true;
}

void UFloatComp::ToggleFloatingActive()
{
	bIsFloatingActive = !bIsFloatingActive;
}

bool UFloatComp::GetIsFloatingActive()
{
	return bIsFloatingActive;
}

void UFloatComp::SetSinkGravity(float grav, float gravAccel)
{
	gravity = grav;
	defaultGravityAccel = gravAccel;
	gravityAccel = defaultGravityAccel;
}


void UFloatComp::BeginPlay()
{
	Super::BeginPlay();
}

void UFloatComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!initalized) { return; }

	auto location = GetComponentLocation();
	auto direction = FVector::UpVector;

	auto speedUpDelta = world->DeltaTimeSeconds * floatBody->GetMass() * massMultiplier;
	auto grav = gravity * gravityAccel * speedUpDelta;
	auto gravForce = FVector(0.f, 0.f, grav);
	floatBody->AddForceAtLocation(gravForce / floatingCompCount, location);

	auto waveHeight = waveHeightMapAsset->GetWaveValue(location, world->GetTimeSeconds());
	if (bIsFloatingActive && location.Z < waveHeight)
	{
		if (!FMath::IsNearlyEqual(gravityAccel, defaultGravityAccel))
		{
			gravityAccel = defaultGravityAccel;
		}

		auto waveDepth = (waveHeight - location.Z) / submergedDivider;
		auto submergedDist = waveDepth * waveDepth;
		auto floatingCounterForce = FMath::Abs(gravity) * submergedDist * speedUpDelta;
		auto posGrav = FMath::Abs(grav);
		auto maxCounterForce = posGrav * floatingCompCount * (submergedDivider/2.f);
		auto forceDir = FVector(0.f, 0.f, FMath::Clamp(floatingCounterForce, 0.f, maxCounterForce));
		floatBody->AddForceAtLocation(forceDir, location);

	}
	else
	{
		gravityAccel += speedUpDelta;
	}
}


