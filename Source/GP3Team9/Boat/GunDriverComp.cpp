
#include "GunDriverComp.h"

#include "BoatEnums.h"
#include "PortGunSlot.h"
#include "ShipGun.h"
#include "DamagableSystem.h"

#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

UGunDriverComp::UGunDriverComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGunDriverComp::SetTarget(AActor* newTarget)
{
	target = newTarget;
}


void UGunDriverComp::BeginPlay()
{
	Super::BeginPlay();


}

void UGunDriverComp::SpawnPortGun(UPortGunSlot* slot, TArray<class AShipGun*>& shipGunList)
{
	auto location = slot->GetComponentLocation();
	auto rotation = slot->GetComponentRotation();
	auto portGunInstance = GetWorld()->SpawnActor<AShipGun>(portGunBP, location, rotation);
	portGunInstance->AttachToComponent(slot, FAttachmentTransformRules::SnapToTargetIncludingScale);
	shipGunList.Add(portGunInstance);
	portGunInstance->Initalize(GetOwner(), slot->sectionPosition);
	allGunSystems.Add(portGunInstance);
}

void UGunDriverComp::AimLeftGuns()
{
	if (!initalized) { return; }
	for (auto gun : leftShipGuns)
	{
		gun->AimGun();
	}
}

void UGunDriverComp::AimRightGuns()
{
	if (!initalized) { return; }
	for (auto gun : rightShipGuns)
	{
		gun->AimGun();
	}
}

void UGunDriverComp::FireLeftGuns()
{
	if (!initalized) { return; }
	for (auto gun : leftShipGuns)
	{
		gun->FireGun();
	}
}

void UGunDriverComp::FireRightGuns()
{
	if (!initalized) { return; }
	for (auto gun : rightShipGuns)
	{
		gun->FireGun();
	}
}


void UGunDriverComp::FireSwivelGun()
{
	if (!initalized) { return; }
	swivelShipGun->FireGun();
}


void UGunDriverComp::FireLeftGunsAtLocation(FVector location)
{
	if (!initalized) { return; }

	auto targetRot = UKismetMathLibrary::FindLookAtRotation(leftShipGuns[0]->GetGunFiringLocation(), location);
	for (auto gun : leftShipGuns)
	{
		gun->SetGunFirePointRotation(targetRot);
		gun->FireGun();
	}
}

void UGunDriverComp::FireRightGunsAtLocation(FVector location)
{
	if (!initalized) { return; }

	auto targetRot = UKismetMathLibrary::FindLookAtRotation(rightShipGuns[0]->GetGunFiringLocation(), location);
	for (auto gun : rightShipGuns)
	{
		gun->SetGunFirePointRotation(targetRot);
		gun->FireGun();
	}
}

void UGunDriverComp::FireSwivelGunAtLocation(FVector location)
{
	if (!initalized) { return; }
	auto targetRot = UKismetMathLibrary::FindLookAtRotation(swivelShipGun->GetGunFiringLocation(), location);
	swivelShipGun->SetGunFirePointRotation(targetRot);
	swivelShipGun->FireGun();
}

TArray<IDamagableSystem*> UGunDriverComp::GetAllGunSystems()
{
	return allGunSystems;
}

void UGunDriverComp::Initalize(USceneComponent* camHolder)
{
	if (!ensure(camHolder)) { return; }
	cameraHolder = camHolder;

	auto owner = GetOwner();
	auto gunSlots = owner->GetComponentsByClass(UPortGunSlot::StaticClass());
	for (auto item : gunSlots)
	{
		auto slot = Cast<UPortGunSlot>(item);

		switch (slot->myPosition)
		{
		default:
			break;
		case EGunSlotPosition::Left:
			leftGunSlots.Add(slot);
			break;
		case EGunSlotPosition::Right:
			rightGunSlots.Add(slot);
			break;
		case EGunSlotPosition::Swivel:
			swivelGunSlot = slot;
			break;
		}
	}
	if (!ensure(swivelGunSlot)) { return; }
	if (!ensure(leftGunSlots.Num() != 0)) { return; }
	if (!ensure(rightGunSlots.Num() != 0)) { return; }

	if (ensure(swivelGunBP))
	{
		auto location = swivelGunSlot->GetComponentLocation();
		auto rotation = swivelGunSlot->GetComponentRotation();
		auto swivelGunInstance = GetWorld()->SpawnActor<AShipGun>(swivelGunBP, location, rotation);
		swivelGunInstance->AttachToComponent(swivelGunSlot, FAttachmentTransformRules::SnapToTargetIncludingScale);
		swivelShipGun = swivelGunInstance;
		swivelShipGun->Initalize(GetOwner(), swivelGunSlot->sectionPosition);
		allGunSystems.Add(swivelShipGun);
	}

	if (ensure(portGunBP))
	{
		for (auto slot : leftGunSlots)
		{
			SpawnPortGun(slot, leftShipGuns);
		}

		for (auto slot : rightGunSlots)
		{
			SpawnPortGun(slot, rightShipGuns);
		}
	}

	swivelDefaultRotation = swivelGunSlot->GetRelativeRotation();
	portGunLeftDefaultYaw = leftGunSlots[0]->GetRelativeRotation().Yaw;
	portGunRightDefaultYaw = rightGunSlots[0]->GetRelativeRotation().Yaw;

	for (auto slot : leftGunSlots)
	{
		slot->SetRelativeRotation(FRotator(0.f, portSlotsLeftNonActiveYaw, 0.f));

	}

	for (auto slot : rightGunSlots)
	{
		slot->SetRelativeRotation(FRotator(0.f, portSlotsRightNonActiveYaw, 0.f));

	}

	initalized = true;
}

void UGunDriverComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!initalized) { return; }
	if (target)
	{
		auto targetRot = UKismetMathLibrary::FindLookAtRotation(swivelGunSlot->GetComponentLocation(), target->GetActorLocation());
		FRotator newYawRot;
		if (targetRot.Yaw > swivelGlobalMinYaw && targetRot.Yaw < swivelGlobalMaxYaw)
		{
			newYawRot = FRotator(0.f, targetRot.Yaw, 0.f);
			swivelGunSlot->SetWorldRotation(newYawRot);
		}

		targetRot = UKismetMathLibrary::FindLookAtRotation(leftGunSlots[0]->GetComponentLocation(), target->GetActorLocation());
		if (targetRot.Yaw > portGunLeftMinYaw && targetRot.Yaw < portGunLeftMaxYaw)
		{
			newYawRot = FRotator(0.f, targetRot.Yaw, 0.f);

			for (auto slot : leftGunSlots)
			{
				slot->SetWorldRotation(newYawRot);
			}
		}
		/*	else
			{

				for (auto slot : leftGunSlots)
				{
					slot->SetRelativeRotation(FRotator(0.f, portSlotsLeftNonActiveYaw, 0.f));
				}
			}*/


		targetRot = UKismetMathLibrary::FindLookAtRotation(rightGunSlots[0]->GetComponentLocation(), target->GetActorLocation());
		if (targetRot.Yaw > portGunRightMinYaw && targetRot.Yaw < portGunRightMaxYaw)
		{
			newYawRot = FRotator(0.f, targetRot.Yaw, 0.f);

			for (auto slot : rightGunSlots)
			{
				slot->SetWorldRotation(newYawRot);
			}
		}
		/*else
		{

			for (auto slot : rightGunSlots)
			{
				slot->SetRelativeRotation(FRotator(0.f, portSlotsRightNonActiveYaw, 0.f));
			}
		}*/


	}
	else
	{
		auto newRot = swivelDefaultRotation + cameraHolder->GetRelativeRotation();
		if (newRot.Yaw > swivelMinYaw && newRot.Yaw < swivelMaxYaw)
		{
			swivelGunSlot->SetRelativeRotation(newRot);
		}
		swivelShipGun->AlignFirePointToHorizon(DeltaTime);

		auto camFow = cameraHolder->GetForwardVector();
		auto cameraFow2D = FVector(camFow.X, camFow.Y, 0.f);

		auto ownerRight = GetOwner()->GetActorRightVector();
		auto ownerRight2D = FVector(ownerRight.X, ownerRight.Y, 0.f);

		auto dot = FVector::DotProduct(cameraFow2D, ownerRight2D);
		UpdatePortGunRotation(
			dot <= -portGunDotAngle,
			DeltaTime,
			portSlotsLeftNonActiveYaw,
			portGunLeftDefaultYaw,
			leftGunSlots);

		UpdatePortGunRotation(
			dot >= portGunDotAngle,
			DeltaTime,
			portSlotsRightNonActiveYaw,
			portGunRightDefaultYaw,
			rightGunSlots);


		for (auto gun : leftShipGuns)
		{
			gun->AlignFirePointToHorizon(DeltaTime);
		}
		for (auto gun :rightShipGuns)
		{
			gun->AlignFirePointToHorizon(DeltaTime);
		}
	}

}

void UGunDriverComp::UpdatePortGunRotation(bool conditon, float DeltaTime, float nonAcitveYaw, float defaultYaw, TArray<UPortGunSlot*> slots)
{
	auto curretnYaw = slots[0]->GetRelativeRotation().Yaw;
	if (conditon)
	{
		for (auto slot : slots)
		{
			slot->SetRelativeRotation((cameraHolder->GetRelativeRotation() + FRotator(0.f, defaultYaw, 0.f)));
		}
	}
	else if (!FMath::IsNearlyEqual(curretnYaw, defaultYaw))
	{
		for (auto slot : slots)
		{
			auto yaw = nonAcitveYaw;//FMath::Lerp(curretnYaw, nonAcitveYaw, DeltaTime * 3.f);
			slot->SetRelativeRotation(FRotator(0.f, yaw, 0.f));

		}
	}
}

