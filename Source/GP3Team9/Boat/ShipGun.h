
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DamagableSystem.h"
#include "ShipGun.generated.h"

UCLASS()
class GP3TEAM9_API AShipGun : public AActor, public IDamagableSystem
{
	GENERATED_BODY()
	
public:	
	AShipGun();
	void Initalize(AActor* boatOwner, EHealthSectionPosition section);
	void FireGun();

	void AimGun();

	UFUNCTION(BlueprintImplementableEvent)
		void OnGunFire();

	UFUNCTION(BlueprintImplementableEvent)
		void OnHit(FVector hitLocation);

	UFUNCTION(BlueprintCallable)
	float GetCoolDownProgress();

	virtual EHealthSectionPosition GetSectionPosition() override;
	virtual void DisableSystem() override;
	virtual void EnableSystem() override;

protected:

	UPROPERTY(EditAnywhere)
	class USceneComponent* firingPoint;

	bool bIsOnCoolDown = false;

	UPROPERTY(EditAnywhere)
		float damage = 200.f;

	UPROPERTY(EditAnywhere)
	float maxCoolDown = 2.f;

	UPROPERTY(EditAnywhere)
	float maxRange = 3000.f;
	//TODO use this value to reduce the damage dealt, probably by a constant vale,
	//if we have time make the drop off be linear after this point
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float damageDropOffRange = 0.7f;

	EHealthSectionPosition sectionPosition;

	float currentCoolDown=0.f;

	AActor* owner;

	bool bAiming = false;
	bool bSystemActive = true;
	bool initalized = false;

public:	
	virtual void Tick(float DeltaTime) override;

};
