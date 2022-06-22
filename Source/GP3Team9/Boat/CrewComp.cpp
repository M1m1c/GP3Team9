
#include "CrewComp.h"

#include "HealthSection.h"

#include "BoatPawn.h"

UCrewComp::UCrewComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCrewComp::Initalize()
{
	totalCrew = crewReserve;

	auto owner = GetOwner();
	if (!ensure(owner)) { return; }
	boatPawn = Cast<ABoatPawn>(owner);
	if (!ensure(boatPawn)) { return; }
	auto sectionColliders = owner->GetComponentsByClass(UHealthSection::StaticClass());

	for (auto item : sectionColliders)
	{
		auto section = Cast<UHealthSection>(item);
		switch (section->SectionPosition)
		{
		default:
			break;
		case EHealthSectionPosition::Left:
			crewableSections.Add(EHealthSectionPosition::Left, section);
			break;
		case EHealthSectionPosition::Right:
			crewableSections.Add(EHealthSectionPosition::Right, section);
			break;
		case EHealthSectionPosition::Front:
			crewableSections.Add(EHealthSectionPosition::Front, section);
			break;
		case EHealthSectionPosition::Back:
			crewableSections.Add(EHealthSectionPosition::Back, section);
			break;
		}
	}

	for (auto startCrew : startingCrewInSections)
	{
		auto key = startCrew.Key;
		if (crewableSections.Contains(key))
		{
			crewableSections[key]->ChangeCrewCount(startCrew.Value);
		}
	}
}

void UCrewComp::MoveCrewMemberX(float xDirection)
{
	if (FMath::IsNearlyZero(xDirection))
	{
		bMovedX = false;
		return;
	}

	if (bCrewLock && (xDirection<0.9f && xDirection>-0.9f)) { return; }

	if (bMovedX) { return; }
	bMovedX = true;
	auto systemPosDirection = xDirection > 0.f ? EHealthSectionPosition::Right : EHealthSectionPosition::Left;
	auto InvertedSystemPosDirection = xDirection > 0.f ? EHealthSectionPosition::Left : EHealthSectionPosition::Right;
	MoveCrew(systemPosDirection, InvertedSystemPosDirection);
}

void UCrewComp::MoveCrewMemberY(float yDirection)
{
	if (FMath::IsNearlyZero(yDirection))
	{
		bMovedY = false;
		return;
	}

	if (bCrewLock && (yDirection<0.9f && yDirection>-0.9f)) { return; }

	if (bMovedY) { return; }
	bMovedY = true;
	auto systemPosDirection = yDirection > 0.f ? EHealthSectionPosition::Front : EHealthSectionPosition::Back;
	auto InvertedSystemPosDirection = yDirection > 0.f ? EHealthSectionPosition::Back : EHealthSectionPosition::Front;
	MoveCrew(systemPosDirection, InvertedSystemPosDirection);
}

void UCrewComp::DisableCrewLock()
{
	bCrewLock = false;
}

void UCrewComp::EnableCrewLock()
{
	bCrewLock = true;
}

void UCrewComp::AddNewCrew(int amountToIncrease)
{
	crewReserve += amountToIncrease;
	totalCrew += amountToIncrease;
	if (totalCrew > crewRequiredToLevel)
	{
		boatPawn->LevelUpBoat();
	}
	boatPawn->OnNewCrewAddedToReserve(amountToIncrease);
}

void UCrewComp::SetCrewCount(int count)
{
	crewReserve = count;
	totalCrew = count;
	for (auto section : crewableSections)
	{
		section.Value->ChangeCrewCount(-100);
	}
	boatPawn->OnNewCrewAddedToReserve(0);
}

int UCrewComp::GetCrewCount()
{
	return totalCrew;
}

void UCrewComp::MoveCrew(EHealthSectionPosition systemPosDirection, EHealthSectionPosition InvertedSystemPosDirection)
{
	auto sectionToMoveTo = crewableSections[systemPosDirection];
	auto invertedSection = crewableSections[InvertedSystemPosDirection];
	if (crewReserve > 0 && sectionToMoveTo->GetCrewCount() < maxCrewInSystem)
	{
		crewReserve--;
		sectionToMoveTo->ChangeCrewCount(1);
		OnSendCrew.Broadcast((int)EHealthSectionPosition::None, (int)systemPosDirection);
	}
	else if (invertedSection->GetCrewCount() > 0)
	{
		crewReserve++;
		invertedSection->ChangeCrewCount(-1);

		OnSendCrew.Broadcast((int)InvertedSystemPosDirection, (int)EHealthSectionPosition::None);
	}
}

void UCrewComp::BeginPlay()
{
	Super::BeginPlay();
}

void UCrewComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	currentRepairTime = FMath::Clamp(currentRepairTime + DeltaTime, 0.f, maxRepairTickTimer);
	if (FMath::IsNearlyEqual(currentRepairTime, maxRepairTickTimer))
	{
		currentRepairTime = 0.f;
		for (auto pair : crewableSections)
		{
			auto section = pair.Value;
			auto maxSectionHealth = section->GetMaxSectionHealth();
			auto currentSectionHealth = section->GetCurrentSectionHealth();
			if (FMath::IsNearlyEqual(maxSectionHealth, currentSectionHealth)) { continue; }

			auto sectionPercentage = maxSectionHealth * repairPercentageIncrease;
			auto repairHealth = sectionPercentage * section->GetCrewCount();
			section->ApplyRepairHealth(repairHealth);
		}
	}
}
