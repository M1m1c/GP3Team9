
#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "BoatEnums.h"
#include "HealthSection.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDestroyedSectionDamage, float, damageTaken);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSectionDestroyedRef, UHealthSection*, sectionDestroyed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSectionRepairedRef, UHealthSection*, sectionRepaired);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GP3TEAM9_API UHealthSection : public UBoxComponent
{
	GENERATED_BODY()
public:
	UHealthSection();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintImplementableEvent)
		void OnDestroyed();
	UFUNCTION(BlueprintImplementableEvent)
		void OnRepaired();

	FDestroyedSectionDamage OnDestroyedDamageTaken;
	FSectionDestroyedRef OnSectionDestroyedWithRef;
	FSectionRepairedRef OnSectionRepairedWithRef;

	UPROPERTY(EditDefaultsOnly)
		EHealthSectionPosition SectionPosition;

	void SectionApplyDamage(float damage);

	void ApplyRepairHealth(float healthRepaired);

	void AddDamagableSystem(class IDamagableSystem* systemToAdd);

	void ChangeCrewCount(int amountToChangeBy);

	int GetCrewCount();

	bool GetIsDestroyed();

	UFUNCTION(BlueprintCallable)
	float GetMaxSectionHealth();

	UFUNCTION(BlueprintCallable)
	float GetCurrentSectionHealth();

protected:
	UPROPERTY(EditDefaultsOnly)
	float maxSectionHealth = 500.f;
	float currentSectionHealth = 500.f;
	int crewCount = 0;
	bool bIsDestroyed = false;
	TArray<class IDamagableSystem*> mySystems;
};
