// Fill out your copyright notice in the Description page of Project Settings.


#include "FloatComp.h"
#include "BoatPawn.h"

#include "GP3Team9/Nelson/WaveHeightmap.h"

UFloatComp::UFloatComp()
{
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UFloatComp::Initalize(UWaveHeightmap* waveMap, float compCount,float grav,float gravAccel,float subMulti,float massMulti)
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

	auto boat = Cast<ABoatPawn>(owner);
	if (!ensure(boat)) { return; }

	boatBody = boat->boatBody;
	if (!ensure(boatBody)) { return; }

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

	auto speedUpDelta = world->DeltaTimeSeconds * boatBody->GetMass() * massMultiplier;

	auto gravForce = FVector(0.f, 0.f, gravity * gravityAccel * speedUpDelta);
	boatBody->AddForceAtLocation(gravForce / floatingCompCount, location);

	auto waveHeight = waveHeightMapAsset->GetWaveValue(location, world->GetTimeSeconds());
	//UE_LOG(LogTemp, Warning, TEXT("%s WaveHeight is: %f"), *GetName(), waveHeight);
	if (bIsFloatingActive && location.Z < waveHeight)
	{
		if (!FMath::IsNearlyEqual(gravityAccel, defaultGravityAccel))
		{
			gravityAccel = defaultGravityAccel;
		}

		auto waveDepth = (waveHeight - location.Z)/ submergedDivider;
		auto submergedDist = waveDepth * waveDepth;
		auto floatingCounterForce = FMath::Abs(gravity) * submergedDist * speedUpDelta;
		auto forceDir = FVector(0.f, 0.f, floatingCounterForce);
		boatBody->AddForceAtLocation(forceDir, location);
	}
	else
	{
		gravityAccel += speedUpDelta;
	}
}


