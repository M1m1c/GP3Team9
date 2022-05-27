
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ShipGun.generated.h"

UCLASS()
class GP3TEAM9_API AShipGun : public AActor
{
	GENERATED_BODY()
	
public:	
	AShipGun();
	void FireGun();

	UFUNCTION(BlueprintImplementableEvent)
		void OnGunFire();

	UFUNCTION(BlueprintCallable)
	float GetCoolDownProgress();

protected:

	UPROPERTY(EditAnywhere)
	class USceneComponent* firingPoint;

	bool bIsOnCoolDown = false;
	UPROPERTY(EditAnywhere)
	float maxCoolDown = 2.f;

	float currentCoolDown=0.f;

public:	
	virtual void Tick(float DeltaTime) override;

};
