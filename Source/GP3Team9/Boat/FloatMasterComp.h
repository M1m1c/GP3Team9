
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FloatMasterComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3TEAM9_API UFloatMasterComp : public UActorComponent
{
	GENERATED_BODY()

public:	

	UFloatMasterComp();

	UPROPERTY(EditAnywhere)
	class UWaveHeightmap* waveHeightMapAsset;

	void DisableFloating();

protected:

	virtual void BeginPlay() override;

	void DisableRandomFloatPoints();

	TArray<class UFloatComp*> floatingComps;

	FTimerHandle DisableFloatingTimer;
	int disabledFloaters = 0;

	//the default gravity force used to push the floatcomps down
	UPROPERTY(EditAnywhere)
	float gravity = -980.f;

	//The default multipler of gravity, used to reset the gravity after acceleration when falling,
	//this multiplied by gravity is the lowest possible gravity force that a floatcomp can experience
	UPROPERTY(EditAnywhere)
	float defaultGravityAccel = 1.f;

	UPROPERTY(EditAnywhere)
	float submergedDivider = 8.f;

	//multiplies the mass of the boatbody, used to affect gravity force and floating counter force
	UPROPERTY(EditAnywhere)
	float massMultiplier = 0.4f;
};
