
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FloatComp.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GP3TEAM9_API UFloatComp : public USceneComponent
{
	GENERATED_BODY()

public:
	UFloatComp();

	void Initalize(class UWaveHeightmap* waveMap, float compCount, float grav, float gravAccel, float subMulti, float massMulti);
	void ToggleFloatingActive();
	bool GetIsFloatingActive();

	void SetSinkGravity(float grav, float gravAccel);
protected:
	virtual void BeginPlay() override;


	class UWaveHeightmap* waveHeightMapAsset;

	float floatingCompCount = 4.f;


	float gravity = -980.f;

	float gravityAccel = 1.f;
	float defaultGravityAccel = 1.f;
	float submergedDivider = 0.25f;
	float massMultiplier = 0.4f;

	class UStaticMeshComponent* boatBody;
	class UWorld* world;

	bool initalized = false;
	bool bIsFloatingActive = true;
	bool bIsSimulating = true;
public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


};
