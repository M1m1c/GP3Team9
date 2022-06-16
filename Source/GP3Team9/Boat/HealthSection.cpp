

#include "HealthSection.h"

#include "DamagableSystem.h"

#include "CameraShakeComp.h"

UHealthSection::UHealthSection()
{
	this->SetCollisionProfileName(FName("DamageCollider"));
}

void UHealthSection::BeginPlay()
{
	Super::BeginPlay();
	currentSectionHealth = maxSectionHealth;
	auto owner = GetOwner();
	if (!ensure(owner)) { return; }
	cameraShaker = Cast<UCameraShakeComp>(owner->GetComponentByClass(UCameraShakeComp::StaticClass()));
	if (!ensure(cameraShaker)) { return; }

}

void UHealthSection::SectionApplyDamage(float damage)
{
	OnSectionHit.Broadcast();
	if (cameraShaker)
	{
		cameraShaker->AddShakeTrauma(0.7f);
	}

	if (bIsDestroyed)
	{
		OnDestroyedDamageTaken.Broadcast(damage);
		return;
	}
	auto calculatedDamage = FMath::Clamp(damage - (sectionArmor * crewCount), minDamage, damage);
	currentSectionHealth = FMath::Clamp(currentSectionHealth - calculatedDamage, 0.f, maxSectionHealth);

	//UE_LOG(LogTemp, Warning, TEXT("%s, %s health: %f"), *GetOwner()->GetName(), *this->GetName(), currentSectionHealth);

	if (FMath::IsNearlyZero(currentSectionHealth))
	{
		OnSectionDestroyedWithRef.Broadcast(this);
		for (auto system : mySystems)
		{
			system->DisableSystem();
		}
		bIsDestroyed = true;
	}
}

void UHealthSection::ApplyRepairHealth(float healthRepaired)
{
	currentSectionHealth = FMath::Clamp(currentSectionHealth + healthRepaired, 0.f, maxSectionHealth);

	if (bIsDestroyed && currentSectionHealth >= (maxSectionHealth * 0.5f))
	{
		UE_LOG(LogTemp, Warning, TEXT("system repaired %s, %s health: %f"), *GetOwner()->GetName(), *this->GetName(), currentSectionHealth);
		bIsDestroyed = false;
		OnSectionRepairedWithRef.Broadcast(this);
		for (auto system : mySystems)
		{
			system->EnableSystem();
		}
	}

	if (!bIsBeingRepaired && currentSectionHealth != maxSectionHealth) { bIsBeingRepaired = true; }
	else if (currentSectionHealth == maxSectionHealth) { bIsBeingRepaired = false; }
}

void UHealthSection::AddDamagableSystem(IDamagableSystem* systemToAdd)
{
	if (!systemToAdd) { return; }
	mySystems.Add(systemToAdd);
}

void UHealthSection::ChangeCrewCount(int amountToChangeBy)
{
	crewCount = FMath::Clamp(crewCount + amountToChangeBy, 0, 10);
	for (auto system : mySystems)
	{
		system->UpdateCrewCount(crewCount);
	}

	if (crewCount == 0) 
	{
		bIsBeingRepaired = false;
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

bool UHealthSection::GetIsBeingRepaired()
{
	return bIsBeingRepaired;
}

float UHealthSection::GetMaxSectionHealth()
{
	return maxSectionHealth;
}

float UHealthSection::GetCurrentSectionHealth()
{
	return currentSectionHealth;
}

FHealthStatus UHealthSection::GetCurrentHealthPercentage()
{
	return FHealthStatus(currentSectionHealth / maxSectionHealth, bIsBeingRepaired, bIsDestroyed);
}
