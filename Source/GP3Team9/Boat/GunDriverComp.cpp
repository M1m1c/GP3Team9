
#include "GunDriverComp.h"

#include "BoatEnums.h"
#include "PortGunSlot.h"
#include "ShipGun.h"
#include "DamagableSystem.h"

#include "Engine/World.h"

UGunDriverComp::UGunDriverComp()
{
	PrimaryComponentTick.bCanEverTick = true;
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

	initalized = true;
}

void UGunDriverComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!initalized) { return; }
	auto newRot = swivelDefaultRotation + cameraHolder->GetRelativeRotation();
	if (newRot.Yaw > swivelMinYaw && newRot.Yaw < swivelMaxYaw)
	{
		swivelGunSlot->SetRelativeRotation(newRot);
	}
}

