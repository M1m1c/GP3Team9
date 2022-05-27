

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
	if (FMath::IsNearlyZero(currentSectionHealth))
	{
		OnDestroyedDamageTaken.Broadcast(damage);
		return;
	}

	currentSectionHealth = FMath::Clamp(currentSectionHealth - damage, 0.f, maxSectionHealth);

	UE_LOG(LogTemp, Warning, TEXT("%s, %s health: %f"), *GetOwner()->GetName(), *this->GetName(), currentSectionHealth);

	if (FMath::IsNearlyZero(currentSectionHealth)) 
	{
		OnSectionDestroyedWithRef.Broadcast(this);
		for (auto system : mySystems)
		{
			system->DisableSystem();
		}
		OnDestroyed();
	}
}

void UHealthSection::AddDamagableSystem(IDamagableSystem* systemToAdd)
{
	if (!systemToAdd) { return; }
	mySystems.Add(systemToAdd);
}
