
#include "TriFireShipGun.h"

#include "DrawDebugHelpers.h"
#include "HealthSection.h"
#include "GP3Team9/ForceFeedback/GP3ForceFeedbackComp.h"

#include "Kismet/KismetMathLibrary.h"

ATriFireShipGun::ATriFireShipGun()
{
	PrimaryActorTick.bCanEverTick = true;
	
	firingPoint_2 = CreateDefaultSubobject<USceneComponent>(FName("FiringPoint_2"));
	firingPoint_2->SetupAttachment(root);

	aimingIndicator_2 = CreateDefaultSubobject<UStaticMeshComponent>(FName("AimingIndicator_2"));
	aimingIndicator_2->SetupAttachment(firingPoint_2);

	firingPoint_3 = CreateDefaultSubobject<USceneComponent>(FName("FiringPoint_3"));
	firingPoint_3->SetupAttachment(root);

	aimingIndicator_3 = CreateDefaultSubobject<UStaticMeshComponent>(FName("AimingIndicator_3"));
	aimingIndicator_3->SetupAttachment(firingPoint_3);
	
	aimingIndicator->SetRelativeLocation(FVector(50.f, 0.f, 0.f));
	aimingIndicator->SetHiddenInGame(true);

	aimingIndicator_2->SetRelativeLocation(FVector(50.f, 0.f, 0.f));
	aimingIndicator_2->SetHiddenInGame(true);

	aimingIndicator_3->SetRelativeLocation(FVector(50.f, 0.f, 0.f));
	aimingIndicator_3->SetHiddenInGame(true);

	firingPoint->SetRelativeScale3D(FVector(90.f, 0.5f, 0.5f));
	firingPoint_2->SetRelativeScale3D(FVector(90.f, 0.5f, 0.5f));
	firingPoint_3->SetRelativeScale3D(FVector(90.f, 0.5f, 0.5f));
}

bool ATriFireShipGun::FireGun()
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
	FireFromFiringPoint(firingPoint);
	FireFromFiringPoint(firingPoint_2);
	FireFromFiringPoint(firingPoint_3);
	return true;
}

void ATriFireShipGun::FireFromFiringPoint(USceneComponent* firePoint)
{
	auto start = firePoint->GetComponentLocation();
	auto forward = firePoint->GetForwardVector();
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
}

void ATriFireShipGun::SetGunFirePointRotation(FRotator rotation)
{
	firingPoint->SetWorldRotation(rotation);
	firingPoint_2->SetWorldRotation(rotation);
	firingPoint_2->SetWorldRotation(rotation);
}

void ATriFireShipGun::HideAimingIndicator(bool bHide)
{
	aimingIndicator->SetHiddenInGame(bHide);
	aimingIndicator_2->SetHiddenInGame(bHide);
	aimingIndicator_3->SetHiddenInGame(bHide);
}

void ATriFireShipGun::AlignFirePointToHorizon(float DeltaTime)
{
	auto speed = DeltaTime * alignmentSpeed;
	auto  firePointRot = firingPoint->GetComponentRotation();
	auto newRot = FQuat(FRotator(0.f, firePointRot.Yaw, 0.f));
	firingPoint->SetWorldRotation(FQuat::Slerp(FQuat(firePointRot), newRot, speed));
	firingPoint_2->SetWorldRotation(FQuat::Slerp(FQuat(firePointRot), newRot, speed));
	firingPoint_3->SetWorldRotation(FQuat::Slerp(FQuat(firePointRot), newRot, speed));
	auto relative = firingPoint->GetRelativeRotation();
	firingPoint->SetRelativeRotation(FRotator(relative.Pitch, 0.f, 0.f));
	firingPoint_2->SetRelativeRotation(FRotator(relative.Pitch, 0.f, 0.f));
	firingPoint_3->SetRelativeRotation(FRotator(relative.Pitch, 0.f, 0.f));
}
