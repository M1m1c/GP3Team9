
#include "ShipGun.h"

#include "DrawDebugHelpers.h"

AShipGun::AShipGun()
{
 	PrimaryActorTick.bCanEverTick = true;
	auto root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(root);
	firingPoint = CreateDefaultSubobject<USceneComponent>(FName("FiringPoint"));
	firingPoint->SetupAttachment(root);
}

void AShipGun::FireGun()
{
	if (bIsOnCoolDown) { return; }
	bIsOnCoolDown = true;
	currentCoolDown = maxCoolDown;
	OnGunFire();
	auto start = firingPoint->GetComponentLocation();
	auto forward = GetActorForwardVector();
	auto end = start + (forward * 3000.f);
	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 2.f, 0, 100.f);
}

float AShipGun::GetCoolDownProgress()
{
	return maxCoolDown/currentCoolDown;
}


void AShipGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsOnCoolDown) 
	{
		currentCoolDown = FMath::Clamp(currentCoolDown - DeltaTime, 0.f, maxCoolDown);
		if (FMath::IsNearlyZero(currentCoolDown)) { bIsOnCoolDown = false; }
	}
}

