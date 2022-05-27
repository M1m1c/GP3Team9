
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBoatDeath);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3TEAM9_API UHealthComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComp();
	void Initalize(class UGunDriverComp* gunDriver);

	FBoatDeath OnBoatDeath;

protected:
	virtual void BeginPlay() override;

	

	TArray<class UHealthSection*> healthSections;
	TArray<class UHealthSection*> destroyedHealthSections;

	UFUNCTION()
	void DestroyedDamageDistribution(float damageTaken);
	UFUNCTION()
	void AddDestroyedSection(class UHealthSection* sectionDestroyed);
	UFUNCTION()
	void RemoveDestroyedSection(class UHealthSection* sectionRepaired);

	bool initalized = false;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
