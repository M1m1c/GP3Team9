
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FloatComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3TEAM9_API UFloatComp : public USceneComponent
{
	GENERATED_BODY()

public:	
	UFloatComp();

	void Initalize(class UWaveHeightmap* waveMap, float compCount);
	void ToggleFloatingActive();
	bool GetIsFloatingActive();
protected:
	virtual void BeginPlay() override;


	class UWaveHeightmap* waveHeightMapAsset;

	float floatingCompCount = 4.f;

	UPROPERTY(EditAnywhere)
		float gravity = -980.f;

	float gravityAccel = 1.f;
	const float defaultGravityAccel = 1.f;
	const float submergedMultiplier = 0.25f;
	const float deltaMultiplier = 0.4f;

	class UStaticMeshComponent* boatBody;
	class UWorld* world;
	
	bool initalized = false;
	bool bIsFloatingActive = true;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
