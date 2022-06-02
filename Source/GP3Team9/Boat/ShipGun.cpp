
#include "ShipGun.h"

#include "DrawDebugHelpers.h"
#include "HealthSection.h"

#include "Kismet/KismetMathLibrary.h"

AShipGun::AShipGun()
{
	PrimaryActorTick.bCanEverTick = true;
	auto root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(root);
	firingPoint = CreateDefaultSubobject<USceneComponent>(FName("FiringPoint"));
	firingPoint->SetupAttachment(root);
}

void AShipGun::Initalize(AActor* boatOwner, EHealthSectionPosition section)
{
	owner = boatOwner;
	if (!ensure(owner)) { return; }
	sectionPosition = section;
	initalized = true;
}

void AShipGun::FireGun()
{
	if (!initalized) { return; }
	if (!bSystemActive) { return; }
	if (bIsOnCoolDown) { return; }
	bIsOnCoolDown = true;
	currentCoolDown = maxCoolDown;
	bAiming = false;
	OnGunFire();
	auto start = firingPoint->GetComponentLocation();
	auto forward = firingPoint->GetForwardVector();
	auto end = start + (forward * maxRange);
	DrawDebugLine(GetWorld(), start, end, FColor::Red, false, 2.f, 0, 100.f);
	TArray<FHitResult> OutHits;
	FCollisionShape MyColSphere = FCollisionShape::MakeSphere(100.0f);
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, start, end, FQuat::Identity, ECC_GameTraceChannel1, MyColSphere);
	if (isHit)
	{
		for (auto hit : OutHits)
		{
			if (hit.Actor == owner) { continue; }
			auto comp = hit.GetComponent();
			auto healthSection = Cast<UHealthSection>(comp);
			if (healthSection)
			{
				healthSection->SectionApplyDamage(damage);
			}
			OnHit(hit.Location);
			DrawDebugSphere(GetWorld(), hit.Location, 100.f, 4, FColor::Purple, false, 2.f);
			break;
		}
	}
}

void AShipGun::AimGun()
{
	bAiming = true;
}

float AShipGun::GetCoolDownProgress()
{
	return maxCoolDown / currentCoolDown;
}

void AShipGun::SetGunFirePointRotation(FRotator rotation)
{
	firingPoint->SetWorldRotation(rotation);
}

FVector AShipGun::GetGunFiringLocation()
{
	return firingPoint->GetComponentLocation();
}

void AShipGun::AlignFirePointToHorizon(float DeltaTime)
{
	auto speed = DeltaTime * alignmentSpeed;
	auto  firePointRot = firingPoint->GetComponentRotation();
	auto newRot = FQuat(FRotator(0.f, firePointRot.Yaw, firePointRot.Roll));
	firingPoint->SetWorldRotation(FQuat::Slerp(FQuat(firePointRot),newRot,speed));
}

EHealthSectionPosition AShipGun::GetSectionPosition()
{
	return sectionPosition;
}

void AShipGun::DisableSystem()
{
	bSystemActive = false;
}

void AShipGun::EnableSystem()
{
	currentCoolDown = 0.f;
	bIsOnCoolDown = false;
	bSystemActive = true;
}

void AShipGun::UpdateCrewCount(int newCrewCount)
{
	crewCount = newCrewCount;
}


void AShipGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!initalized) { return; }
	if (!bSystemActive) { return; }
	if (bIsOnCoolDown)
	{
		currentCoolDown = FMath::Clamp(currentCoolDown - (DeltaTime + (DeltaTime * crewCount)), 0.f, maxCoolDown);
		if (FMath::IsNearlyZero(currentCoolDown)) { bIsOnCoolDown = false; }
	}

	if (bAiming) {
		auto start = firingPoint->GetComponentLocation();
		auto forward = GetActorForwardVector();
		auto end = start + (forward * maxRange);
		//DrawDebugLine(GetWorld(), start, end, FColor(255.f,255.f,255.f,0.2f), false, -1.f, 0, 100.f);
		//TODO draw transperent aiming line
	}
}

