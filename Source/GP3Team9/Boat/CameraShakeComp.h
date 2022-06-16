
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraShakeComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3TEAM9_API UCameraShakeComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCameraShakeComp();

	void Initalize();
	void AddShakeTrauma(float traumaToAdd);
	float GetTrauma();
protected:
	virtual void BeginPlay() override;
	void ShakeCamera(float DeltaTime);

	float Trauma = 0.f;
	float maxTrauma = 1.f;
	float DegerdationRateTrauma = 2.0f;
	float MaxRotation = 1.0f;

	bool bInitalized = false;

	class UCameraComponent* camera;
	FRotator defaultRotation;
public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
