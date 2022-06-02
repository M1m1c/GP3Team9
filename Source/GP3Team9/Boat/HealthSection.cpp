

#include "HealthSection.h"

#include "DamagableSystem.h"

UHealthSection::UHealthSection()
{
	this->SetCollisionProfileName(FName("DamageCollider"));
}

void UHealthSection::BeginPlay()
{
	Super::BeginPlay();
	currentSectionHealth = maxSectionHealth;
}

void UHealthSection::SectionApplyDamage(float damage)
{
	if (bIsDestroyed)
	{
		OnDestroyedDamageTaken.Broadcast(damage);
		return;
	}

	currentSectionHealth = FMath::Clamp(currentSectionHealth - damage, 0.f, maxSectionHealth);

	//UE_LOG(LogTemp, Warning, TEXT("%s, %s health: %f"), *GetOwner()->GetName(), *this->GetName(), currentSectionHealth);
	
	if (FMath::IsNearlyZero(currentSectionHealth))
	{
		OnSectionDestroyedWithRef.Broadcast(this);
		for (auto system : mySystems)
		{
			system->DisableSystem();
		}
		bIsDestroyed = true;
		OnDestroyed();
	}
}

void UHealthSection::ApplyRepairHealth(float healthRepaired)
{
	currentSectionHealth = FMath::Clamp(currentSectionHealth + healthRepaired, 0.f, maxSectionHealth);
	
	if (bIsDestroyed && currentSectionHealth >= (maxSectionHealth * 0.5f))
	{
		UE_LOG(LogTemp, Warning, TEXT("system repaired %s, %s health: %f"), *GetOwner()->GetName(), *this->GetName(), currentSectionHealth);
		bIsDestroyed = false;
		OnRepaired();
		OnSectionRepairedWithRef.Broadcast(this);
		for (auto system : mySystems)
		{
			system->EnableSystem();
		}
	}
}

void UHealthSection::AddDamagableSystem(IDamagableSystem* systemToAdd)
{
	if (!systemToAdd) { return; }
	mySystems.Add(systemToAdd);
}

void UHealthSection::ChangeCrewCount(int amountToChangeBy)
{
	crewCount += amountToChangeBy;
	for (auto system : mySystems) 
	{
		system->UpdateCrewCount(crewCount);
	}
}

int UHealthSection::GetCrewCount()
{
	return crewCount;
}

bool UHealthSection::GetIsDestroyed()
{
	return bIsDestroyed;
}

float UHealthSection::GetMaxSectionHealth()
{
	return maxSectionHealth;
}

float UHealthSection::GetCurrentSectionHealth()
{
	return currentSectionHealth;
}
