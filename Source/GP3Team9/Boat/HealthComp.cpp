
#include "HealthComp.h"

#include "GunDriverComp.h"
#include "HealthSection.h"
#include "DamagableSystem.h"

UHealthComp::UHealthComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthComp::Initalize(UGunDriverComp* gunDriver)
{

	auto owner = GetOwner();
	auto sectionColliders = owner->GetComponentsByClass(UHealthSection::StaticClass());

	for (auto item : sectionColliders)
	{
		healthSections.Add(Cast<UHealthSection>(item));
	}

	auto sectionSystems = owner->GetComponentsByInterface(UDamagableSystem::StaticClass());
	TArray<IDamagableSystem*> damagableSystems;
	for (auto item : sectionSystems)
	{
		damagableSystems.Add(Cast<IDamagableSystem>(item));
	}

	auto guns = gunDriver->GetAllGunSystems();
	for (auto item : guns)
	{
		damagableSystems.Add(item);
	}


	for (int i = 0; i < healthSections.Num(); i++)
	{
		auto section = healthSections[i];
		auto sectionPos = section->SectionPosition;
		for (int g = 0; g < damagableSystems.Num(); g++)
		{
			auto system = damagableSystems[g];
			if (system->GetSectionPosition() == sectionPos)
			{
				section->AddDamagableSystem(system);
			}
		}
		section->OnSectionDestroyedWithRef.AddDynamic(this, &UHealthComp::AddDestroyedSection);
		section->OnSectionRepairedWithRef.AddDynamic(this, &UHealthComp::RemoveDestroyedSection);
		section->OnDestroyedDamageTaken.AddDynamic(this, &UHealthComp::DestroyedDamageDistribution);
	}

	initalized = true;
}

void UHealthComp::BeginPlay()
{
	Super::BeginPlay();
}


void UHealthComp::DestroyedDamageDistribution(float damageTaken)
{
	//TODO if sections destroyed equals health sections num then kill boat
	if (healthSections.Num() == destroyedHealthSections.Num()) 
	{
		UE_LOG(LogTemp, Warning, TEXT("BOAT DESTROYED!"));
		OnBoatDeath.Broadcast();
		return;
	}

	for (auto section : healthSections)
	{
		if (destroyedHealthSections.Num() != 0)
		{
			if (destroyedHealthSections.Contains(section)) { continue; }
		}
		section->SectionApplyDamage(damageTaken);
	}
}

void UHealthComp::AddDestroyedSection(UHealthSection* sectionDestroyed)
{
	if (!sectionDestroyed) { return; }
	if (destroyedHealthSections.Num() != 0) 
	{
		if (destroyedHealthSections.Contains(sectionDestroyed)) { return; }
	}
	destroyedHealthSections.Add(sectionDestroyed);
}

void UHealthComp::RemoveDestroyedSection(UHealthSection* sectionRepaired)
{
	if (!sectionRepaired) { return; }
	if (destroyedHealthSections.Num() != 0) 
	{
		if (!destroyedHealthSections.Contains(sectionRepaired)) { return; }
	}
	destroyedHealthSections.Remove(sectionRepaired);
}

void UHealthComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

