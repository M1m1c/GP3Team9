
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FBoatDeath);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSecHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSecDestroyedRef, class UHealthSection*, sectionDestroyed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSecRepairedRef, class UHealthSection*, sectionRepaired);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3TEAM9_API UHealthComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	UHealthComp();
	void Initalize(class UGunDriverComp* gunDriver);

	FBoatDeath OnBoatDeath;
	FSecDestroyedRef OnSectionDestroyed;
	FSecRepairedRef OnSectionRepaired;
	FSecHit OnSectionHit;

	UFUNCTION(BlueprintCallable)
	void ApplyDamageToAllSections(float damage);

protected:
	virtual void BeginPlay() override;	
	UPROPERTY(BlueprintReadOnly)
	TArray<class UHealthSection*> healthSections;
	UPROPERTY(BlueprintReadOnly)
	TArray<class UHealthSection*> destroyedHealthSections;

	UFUNCTION()
	void DestroyedDamageDistribution(float damageTaken);
	UFUNCTION()
	void AddDestroyedSection(class UHealthSection* sectionDestroyed);
	UFUNCTION()
	void RemoveDestroyedSection(class UHealthSection* sectionRepaired);

	UFUNCTION()
		void SectionHit();

	bool initalized = false;	
};
