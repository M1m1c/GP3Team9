
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
};
