
#include "GunDriverComp.h"

#include "PortGunSlot.h"
#include "ShipGun.h"
#include "DamagableSystem.h"
#include "CameraShakeComp.h"

#include "Engine/World.h"
#include "Kismet/KismetMathLibrary.h"

UGunDriverComp::UGunDriverComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGunDriverComp::SetTarget(AController* newTarget)
{
	target = newTarget;
	if (!swivelShipGun) { return; }
	auto aiming = swivelShipGun->aimingIndicator;
	if (!aiming) { return; }
	aiming->SetHiddenInGame(true);
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
		if (!gun) { continue; }
		gun->AimGun();
	}
}

void UGunDriverComp::AimRightGuns()
{
	if (!initalized) { return; }
	for (auto gun : rightShipGuns)
	{
		if (!gun) { continue; }
		gun->AimGun();
	}
}

void UGunDriverComp::FireLeftGuns()
{
	if (!initalized) { return; }
	if (!leftShipGuns.Num()) { return; }
	if (leftShipGuns[0]->bAutomaticFire && !target)
	{
		bFiringLeft = true;
		return;
	}
	FireArrayOfGuns(leftShipGuns, leftShipGuns[0]->ShakeTrauma);
}

void UGunDriverComp::FireRightGuns()
{
	if (!initalized) { return; }
	if (!rightShipGuns.Num()) { return; }
	if (rightShipGuns[0]->bAutomaticFire && !target)
	{
		bFiringRight = true;
		return;
	}
	FireArrayOfGuns(rightShipGuns, rightShipGuns[0]->ShakeTrauma);
}

void UGunDriverComp::FireSwivelGun()
{
	if (!initalized) { return; }
	if (!swivelShipGun) { return; }
	if (swivelShipGun->bAutomaticFire && !target)
	{
		bFiringSwivel = true;
		return;
	}
	FireSingleShipGun(swivelShipGun, swivelShipGun->ShakeTrauma);
}

void UGunDriverComp::FireArrayOfGuns(TArray<AShipGun*> guns, float traumaToAdd)
{
	for (auto gun : guns)
	{
		if (!gun) { continue; }
		FireSingleShipGun(gun, traumaToAdd);
	}
}

void UGunDriverComp::FireSingleShipGun(AShipGun* gun, float traumaToAdd)
{
	if (!gun) { return; }
	if (gun->FireGun())
	{
		cameraShaker->AddShakeTrauma(traumaToAdd);
	}
}

void UGunDriverComp::StopFireLeftGuns()
{
	bFiringLeft = false;
}

void UGunDriverComp::StopFireRightGuns()
{
	bFiringRight = false;
}

void UGunDriverComp::StopFireSwivelGun()
{
	bFiringSwivel = false;
}


void UGunDriverComp::FireLeftGunsAtLocation(FVector location)
{
	if (!initalized) { return; }
	if (!leftShipGuns.Num()) { return; }
	auto targetRot = UKismetMathLibrary::FindLookAtRotation(leftShipGuns[0]->GetGunFiringLocation(), location);
	for (auto gun : leftShipGuns)
	{
		if (!gun) { continue; }
		gun->SetGunFirePointRotation(targetRot);
		gun->FireGun();
	}
}

void UGunDriverComp::FireRightGunsAtLocation(FVector location)
{
	if (!initalized) { return; }
	if (!rightShipGuns.Num()) { return; }
	auto targetRot = UKismetMathLibrary::FindLookAtRotation(rightShipGuns[0]->GetGunFiringLocation(), location);
	for (auto gun : rightShipGuns)
	{
		if (!gun) { continue; }
		gun->SetGunFirePointRotation(targetRot);
		gun->FireGun();
	}
}

void UGunDriverComp::FireSwivelGunAtLocation(FVector location)
{
	if (!initalized) { return; }
	if (!swivelShipGun) { return; }
	auto targetRot = UKismetMathLibrary::FindLookAtRotation(swivelShipGun->GetGunFiringLocation(), location);
	swivelShipGun->SetGunFirePointRotation(targetRot);
	swivelShipGun->FireGun();
}

TArray<IDamagableSystem*> UGunDriverComp::GetAllGunSystems()
{
	return allGunSystems;
}

TMap<EGunSlotPosition, class AShipGun*> UGunDriverComp::GetEachDirectionShipGuns()
{
	return eachDirectionShipGuns;
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


	if (swivelGunSlot)
	{
		if (!ensure(swivelGunBP)) { return; }
		auto location = swivelGunSlot->GetComponentLocation();
		auto rotation = swivelGunSlot->GetComponentRotation();
		auto swivelGunInstance = GetWorld()->SpawnActor<AShipGun>(swivelGunBP, location, rotation);
		swivelGunInstance->AttachToComponent(swivelGunSlot, FAttachmentTransformRules::SnapToTargetIncludingScale);
		swivelShipGun = swivelGunInstance;
		swivelShipGun->Initalize(GetOwner(), swivelGunSlot->sectionPosition);
		allGunSystems.Add(swivelShipGun);
		swivelDefaultRotation = swivelGunSlot->GetRelativeRotation();
		eachDirectionShipGuns.Add(EGunSlotPosition::Swivel, swivelShipGun);
	}

	if (leftGunSlots.Num())
	{
		if (!ensure(portGunBP)) { return; }
		for (auto slot : leftGunSlots)
		{
			SpawnPortGun(slot, leftShipGuns);
		}
		portGunLeftDefaultYaw = leftGunSlots[0]->GetRelativeRotation().Yaw;
		for (auto slot : leftGunSlots)
		{
			slot->SetRelativeRotation(FRotator(0.f, portSlotsLeftNonActiveYaw, 0.f));

		}
		eachDirectionShipGuns.Add(EGunSlotPosition::Left, leftShipGuns[0]);
	}

	if (rightGunSlots.Num())
	{
		if (!ensure(portGunBP)) { return; }
		for (auto slot : rightGunSlots)
		{
			SpawnPortGun(slot, rightShipGuns);
		}
		portGunRightDefaultYaw = rightGunSlots[0]->GetRelativeRotation().Yaw;
		for (auto slot : rightGunSlots)
		{
			slot->SetRelativeRotation(FRotator(0.f, portSlotsRightNonActiveYaw, 0.f));

		}
		eachDirectionShipGuns.Add(EGunSlotPosition::Right, rightShipGuns[0]);
	}

	if (!ensure(eachDirectionShipGuns.Num() != 0)) { return; }

	cameraShaker = Cast<UCameraShakeComp>(owner->GetComponentByClass(UCameraShakeComp::StaticClass()));
	if (!ensure(cameraShaker)) { return; }

	if (target)
	{
		if (!swivelShipGun) { return; }
		auto aiming = swivelShipGun->aimingIndicator;
		if (!aiming) { return; }
		aiming->SetHiddenInGame(true);
	}

	initalized = true;
}

void UGunDriverComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!initalized) { return; }
	if (target)
	{
		RotateGunsToFaceTargetController();
	}
	else
	{
		RotateGunsToLookWithCamera(DeltaTime);


		for (auto gun : leftShipGuns)
		{
			if (!gun) { continue; }
			gun->AlignFirePointToHorizon(DeltaTime);
		}
		for (auto gun : rightShipGuns)
		{
			if (!gun) { continue; }
			gun->AlignFirePointToHorizon(DeltaTime);
		}

		if (bFiringLeft)
		{
			FireArrayOfGuns(leftShipGuns, 0.5f);
		}

		if (bFiringRight)
		{
			FireArrayOfGuns(rightShipGuns, 0.5f);
		}

		if (bFiringSwivel)
		{
			FireSingleShipGun(swivelShipGun, 1.f);
		}
	}
}

void UGunDriverComp::RotateGunsToFaceTargetController()
{
	auto pawn = target->GetPawn();
	if (!pawn) { return; }
	auto location = pawn->GetActorLocation();
	auto targetRot = UKismetMathLibrary::FindLookAtRotation(swivelGunSlot->GetComponentLocation(), location);
	FRotator newYawRot;
	
	if(swivelGunSlot)
	{
		if (targetRot.Yaw > swivelGlobalMinYaw && targetRot.Yaw < swivelGlobalMaxYaw)
		{
			newYawRot = FRotator(0.f, targetRot.Yaw, 0.f);
			swivelGunSlot->SetWorldRotation(newYawRot);
		}
	}
	

	if (leftGunSlots.Num())
	{
		targetRot = UKismetMathLibrary::FindLookAtRotation(leftGunSlots[0]->GetComponentLocation(), location);
		if (targetRot.Yaw > portGunLeftMinYaw && targetRot.Yaw < portGunLeftMaxYaw)
		{
			newYawRot = FRotator(0.f, targetRot.Yaw, 0.f);

			for (auto slot : leftGunSlots)
			{
				if (!slot) { continue; }
				slot->SetWorldRotation(newYawRot);
			}
		}
	}


	if(rightGunSlots.Num())
	{
		targetRot = UKismetMathLibrary::FindLookAtRotation(rightGunSlots[0]->GetComponentLocation(), location);
		if (targetRot.Yaw > portGunRightMinYaw && targetRot.Yaw < portGunRightMaxYaw)
		{
			newYawRot = FRotator(0.f, targetRot.Yaw, 0.f);

			for (auto slot : rightGunSlots)
			{
				if (!slot) { continue; }
				slot->SetWorldRotation(newYawRot);
			}
		}
	}
	

}

void UGunDriverComp::RotateGunsToLookWithCamera(float DeltaTime)
{
	auto newRot = swivelDefaultRotation + cameraHolder->GetRelativeRotation();
	if (newRot.Yaw > swivelMinYaw && newRot.Yaw < swivelMaxYaw)
	{
		swivelGunSlot->SetRelativeRotation(newRot);
		swivelShipGun->aimingIndicator->SetHiddenInGame(false);
	}
	else
	{

		swivelShipGun->aimingIndicator->SetHiddenInGame(true);
	}
	swivelShipGun->AlignFirePointToHorizon(DeltaTime);

	auto camFow = cameraHolder->GetForwardVector();
	auto cameraFow2D = FVector(camFow.X, camFow.Y, 0.f);

	auto ownerRight = GetOwner()->GetActorRightVector();
	auto ownerRight2D = FVector(ownerRight.X, ownerRight.Y, 0.f);

	auto dot = FVector::DotProduct(cameraFow2D, ownerRight2D);
	auto bRotateLeftGuns = dot < -portGunDotAngle;
	UpdatePortGunRotation(
		bRotateLeftGuns,
		DeltaTime,
		portSlotsLeftNonActiveYaw,
		portGunLeftDefaultYaw,
		leftGunSlots);
	UpdateShowAimingIndicator(bRotateLeftGuns, leftShipGuns);

	auto bRotateRightGuns = dot > portGunDotAngle;
	UpdatePortGunRotation(
		bRotateRightGuns,
		DeltaTime,
		portSlotsRightNonActiveYaw,
		portGunRightDefaultYaw,
		rightGunSlots);
	UpdateShowAimingIndicator(bRotateRightGuns, rightShipGuns);
}

void UGunDriverComp::DestroyGuns()
{
	initalized = false;
	if (swivelShipGun) {
		swivelShipGun->Destroy();
	}

	for (auto gun : leftShipGuns)
	{
		if (gun) { gun->Destroy(); }
	}

	for (auto gun : rightShipGuns)
	{
		if (gun) { gun->Destroy(); }
	}
}

void UGunDriverComp::UpdateShowAimingIndicator(bool condition, TArray<AShipGun*> guns)
{
	if (!guns.Num()) { return; }
	for (auto gun : guns)
	{
		if (!gun) { continue; }
		if (condition)
		{
			gun->aimingIndicator->SetHiddenInGame(false);
		}
		else
		{
			gun->aimingIndicator->SetHiddenInGame(true);
		}
	}
}

void UGunDriverComp::UpdatePortGunRotation(bool conditon, float DeltaTime, float nonAcitveYaw, float defaultYaw, TArray<UPortGunSlot*> slots)
{
	if (!slots.Num()) { return; }
	auto curretnYaw = slots[0]->GetRelativeRotation().Yaw;
	if (conditon)
	{
		for (auto slot : slots)
		{
			if (!slot) { continue; }
			slot->SetRelativeRotation((cameraHolder->GetRelativeRotation() + FRotator(0.f, defaultYaw, 0.f)));
		}
	}
}

