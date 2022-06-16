
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
	bool FireGun();

	void AimGun();

	UFUNCTION(BlueprintImplementableEvent)
		void OnGunFire();

	UFUNCTION(BlueprintImplementableEvent)
		void OnHit(FVector hitLocation);

	UFUNCTION(BlueprintCallable)
		//returns 1 to 0 with. 0 being no cooldwon and 1 being full cooldown.
	float GetCoolDownProgress();

	void SetGunFirePointRotation(FRotator rotation);
	FVector GetGunFiringLocation();

	void AlignFirePointToHorizon(float DeltaTime);

	virtual EHealthSectionPosition GetSectionPosition() override;
	virtual void DisableSystem() override;
	virtual void EnableSystem() override;
	virtual void UpdateCrewCount(int newCrewCount) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* aimingIndicator;

	UPROPERTY(EditDefaultsOnly)
	bool bAutomaticFire = false;
	UPROPERTY(EditDefaultsOnly)
	float ShakeTrauma = 1.f;

protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class USceneComponent* firingPoint;

	bool bIsOnCoolDown = false;

	UPROPERTY(EditAnywhere)
	float damage = 200.f;

	UPROPERTY(EditAnywhere)
	float maxCoolDown = 2.f;

	UPROPERTY(EditAnywhere)
	float maxRange = 4500.f;

	UPROPERTY(EditAnywhere)
	float alignmentSpeed = 5.f;
	//TODO use this value to reduce the damage dealt, probably by a constant vale,
	//if we have time make the drop off be linear after this point
	//UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	//float damageDropOffRange = 0.7f;

	float currentCoolDown=0.f;

	int crewCount = 0;

	EHealthSectionPosition sectionPosition;

	AActor* owner;

	bool bAiming = false;
	bool bSystemActive = true;
	bool initalized = false;

public:	
	virtual void Tick(float DeltaTime) override;

};
