
#include "ShipGun.h"

#include "DrawDebugHelpers.h"
#include "HealthSection.h"
#include "GP3Team9/ForceFeedback/GP3ForceFeedbackComp.h"

#include "Kismet/KismetMathLibrary.h"

AShipGun::AShipGun()
{
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(root);
	firingPoint = CreateDefaultSubobject<USceneComponent>(FName("FiringPoint"));
	firingPoint->SetupAttachment(root);

	aimingIndicator = CreateDefaultSubobject<UStaticMeshComponent>(FName("AimingIndicator"));
	aimingIndicator->SetupAttachment(firingPoint);

	forceFeedbackComp = CreateDefaultSubobject<UGP3ForceFeedbackComp>(TEXT("ForceFeedbackComp"));

	aimingIndicator->SetRelativeLocation(FVector(50.f, 0.f, 0.f));
	aimingIndicator->SetHiddenInGame(true);
	firingPoint->SetRelativeScale3D(FVector(90.f, 0.5f, 0.5f));
}

void AShipGun::Initalize(AActor* boatOwner, EHealthSectionPosition section)
{
	aimingIndicator->SetCollisionProfileName(FName("NoCollision"));
	owner = boatOwner;
	if (!ensure(owner)) { return; }

	auto pawn = Cast<APawn>(owner);
	if (pawn) {
		if (Cast<APlayerController>(pawn->GetController())) 
		{
			bIsControlledByPlayer = true;
		}
	}

	if (!bIsControlledByPlayer) 
	{
		HideAimingIndicator(true);
	}

	sectionPosition = section;
	initalized = true;
}

void AShipGun::SetIsControlledByPlayer(bool bValue)
{
	bIsControlledByPlayer = bValue;
}

bool AShipGun::FireGun()
{
	if (!initalized) { return false; }
	if (crewCount == 0) { return false; }
	if (!bSystemActive) { return false; }
	if (bIsOnCoolDown) { return false; }
	if (!bCanFire) { return false; }
	bIsOnCoolDown = true;
	currentCoolDown = maxCoolDown;
	bAiming = false;
	OnGunFire();
	auto start = firingPoint->GetComponentLocation();
	auto forward = firingPoint->GetForwardVector();
	auto end = start + (forward * maxRange);
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
			break;
		}
	}
	return true;
}

void AShipGun::AimGun()
{
	//bAiming = true;
}

float AShipGun::GetCoolDownProgress()
{
	return currentCoolDown / maxCoolDown;
}

void AShipGun::SetGunFirePointRotation(FRotator rotation)
{
	firingPoint->SetWorldRotation(rotation);
}

FVector AShipGun::GetGunFiringLocation()
{
	return firingPoint->GetComponentLocation();
}

void AShipGun::HideAimingIndicator(bool bHide)
{
	aimingIndicator->SetHiddenInGame(bHide);
}

void AShipGun::AlignFirePointToHorizon(float DeltaTime)
{
	auto speed = DeltaTime * alignmentSpeed;
	auto  firePointRot = firingPoint->GetComponentRotation();
	auto newRot = FQuat(FRotator(0.f, firePointRot.Yaw, 0.f));
	firingPoint->SetWorldRotation(FQuat::Slerp(FQuat(firePointRot), newRot, speed));
	auto relative = firingPoint->GetRelativeRotation();
	firingPoint->SetRelativeRotation(FRotator(relative.Pitch, 0.f, 0.f));
}

EHealthSectionPosition AShipGun::GetSectionPosition()
{
	return sectionPosition;
}

void AShipGun::DisableSystem()
{
	bSystemActive = false;
	if (bIsControlledByPlayer)
	{
		HideAimingIndicator(true);
		bCanShowIndicator = false;
	}
}

void AShipGun::EnableSystem()
{
	currentCoolDown = 0.f;
	bIsOnCoolDown = false;
	bSystemActive = true;
	if (bIsControlledByPlayer)
	{
		HideAimingIndicator(false);
		bCanShowIndicator = true;
	}
}

void AShipGun::UpdateCrewCount(int newCrewCount)
{
	crewCount = newCrewCount;
	if (bIsControlledByPlayer)
	{
		if (bSystemActive && crewCount == 0 && !aimingIndicator->bHiddenInGame)
		{
			HideAimingIndicator(true);
			bCanShowIndicator = false;
		}
		else if (bSystemActive && crewCount > 0 && aimingIndicator->bHiddenInGame)
		{
			HideAimingIndicator(false);
			bCanShowIndicator = true;
		}
	}
	
}


AActor* AShipGun::GetOwningBoat()
{
	if (!initalized) { return nullptr; }
	return owner;
}

void AShipGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!initalized) { return; }
	if (!bSystemActive) { return; }
	if (bIsOnCoolDown)
	{
		auto crewBonus = ((DeltaTime * FMath::Clamp(crewCount - 1, 0, crewCount))) * 0.5f;
		currentCoolDown = FMath::Clamp(
			currentCoolDown - (DeltaTime + crewBonus),
			0.f,
			maxCoolDown);
		if (FMath::IsNearlyZero(currentCoolDown)) { bIsOnCoolDown = false; }
	}
}

