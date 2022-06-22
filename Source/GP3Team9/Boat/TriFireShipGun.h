
#pragma once

#include "CoreMinimal.h"
#include "ShipGun.h"
#include "DamagableSystem.h"
#include "TriFireShipGun.generated.h"

UCLASS()
class GP3TEAM9_API ATriFireShipGun : public AShipGun
{
	GENERATED_BODY()
	
public:	
	ATriFireShipGun();
	virtual bool FireGun() override;

	void FireFromFiringPoint(USceneComponent* firePoint);

	virtual void SetGunFirePointRotation(FRotator rotation) override;

	virtual void HideAimingIndicator(bool bHide);

	virtual void AlignFirePointToHorizon(float DeltaTime) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* aimingIndicator_2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* aimingIndicator_3;
protected:

	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	class USceneComponent* firingPoint_2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class USceneComponent* firingPoint_3;

};
