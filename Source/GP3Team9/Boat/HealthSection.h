
#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "BoatEnums.h"
#include "HealthSection.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSectionHit);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDestroyedSectionDamage, float, damageTaken);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSectionDestroyedRef, UHealthSection*, sectionDestroyed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSectionRepairedRef, UHealthSection*, sectionRepaired);

USTRUCT(BlueprintType, Blueprintable)
struct GP3TEAM9_API FHealthStatus
{
	GENERATED_USTRUCT_BODY()
public:
	FHealthStatus() {};
	FHealthStatus(float health, bool repair, bool destro) 
	{
		HealthPercentage = health;
		bIsRepairing = repair;
		bIsDestroyed = destro;
	};

	UPROPERTY(BlueprintReadOnly)
	float HealthPercentage;

	UPROPERTY(BlueprintReadOnly)
	bool bIsRepairing;

	UPROPERTY(BlueprintReadOnly)
	bool bIsDestroyed;
};

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GP3TEAM9_API UHealthSection : public UBoxComponent
{
	GENERATED_BODY()
public:
	UHealthSection();
	virtual void BeginPlay() override;

	FDestroyedSectionDamage OnDestroyedDamageTaken;
	FSectionDestroyedRef OnSectionDestroyedWithRef;
	FSectionRepairedRef OnSectionRepairedWithRef;
	FSectionHit OnSectionHit;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly)
		EHealthSectionPosition SectionPosition;

	void SectionApplyDamage(float damage);

	void ApplyRepairHealth(float healthRepaired);

	void AddDamagableSystem(class IDamagableSystem* systemToAdd);

	void ChangeCrewCount(int amountToChangeBy);

	UFUNCTION(BlueprintCallable)
	int GetCrewCount();

	UFUNCTION(BlueprintCallable)
	bool GetIsDestroyed();

	UFUNCTION(BlueprintCallable)
	bool GetIsBeingRepaired();

	UFUNCTION(BlueprintCallable)
	float GetMaxSectionHealth();

	UFUNCTION(BlueprintCallable)
	float GetCurrentSectionHealth();

	UFUNCTION(BlueprintCallable)
	FHealthStatus GetCurrentHealthPercentage();
	

protected:

	UPROPERTY(EditDefaultsOnly)
	float minDamage = 10.f;

	UPROPERTY(EditDefaultsOnly)
	float maxSectionHealth = 500.f;
	float currentSectionHealth = 500.f;

	float sectionArmor = 10.f;
	int crewCount = 0;
	bool bIsDestroyed = false;
	bool bIsBeingRepaired = false;
	TArray<class IDamagableSystem*> mySystems;

	class UCameraShakeComp* cameraShaker;
};
