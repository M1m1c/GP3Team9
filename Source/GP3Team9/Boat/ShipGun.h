
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
	void SetIsControlledByPlayer(bool bValue);
	virtual bool FireGun();

	void AimGun();

	UFUNCTION(BlueprintImplementableEvent)
		void OnGunFire();

	UFUNCTION(BlueprintImplementableEvent)
		void OnHit(FVector hitLocation);

	UFUNCTION(BlueprintCallable)
		//returns 1 to 0 with. 0 being no cooldwon and 1 being full cooldown.
	float GetCoolDownProgress();

	virtual void SetGunFirePointRotation(FRotator rotation);
	FVector GetGunFiringLocation();

	virtual void HideAimingIndicator(bool bHide);
	virtual void AlignFirePointToHorizon(float DeltaTime);

	virtual EHealthSectionPosition GetSectionPosition() override;
	virtual void DisableSystem() override;
	virtual void EnableSystem() override;
	virtual void UpdateCrewCount(int newCrewCount) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* aimingIndicator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UGP3ForceFeedbackComp* forceFeedbackComp;

	UPROPERTY(EditDefaultsOnly)
	bool bAutomaticFire = false;
	UPROPERTY(EditDefaultsOnly)
	float ShakeTrauma = 1.f;

	bool bAiming = false;
	bool bCanFire = true;
	bool bCanShowIndicator = false;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USceneComponent* root;

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

	float currentCoolDown=0.f;

	int crewCount = 0;

	EHealthSectionPosition sectionPosition;

	AActor* owner;

	bool bSystemActive = true;
	bool initalized = false;

	bool bIsControlledByPlayer = false;
public:	
	UFUNCTION(BlueprintCallable)
	AActor* GetOwningBoat();
	virtual void Tick(float DeltaTime) override;

};
