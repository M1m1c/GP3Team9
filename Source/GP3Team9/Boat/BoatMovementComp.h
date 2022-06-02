
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamagableSystem.h"
#include "BoatMovementComp.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3TEAM9_API UBoatMovementComp : public UActorComponent, public IDamagableSystem
{
	GENERATED_BODY()

public:	
	UBoatMovementComp();

	void Initalize();

	bool Initalized = false;

	void AllowGearChange();
	void DisallowGearChange();

	void ReadGearChange(float value);

	void SetGear(int newGear);

	void ReadTurning(float value);

	virtual EHealthSectionPosition GetSectionPosition() override;
	virtual void DisableSystem() override;
	virtual void EnableSystem() override;
	virtual void UpdateCrewCount(int newCrewCount) override;
	
	UPROPERTY(EditDefaultsOnly)
	float turnSpeed = 80.f;

protected:

	float GetUpdatedRotAxis(float DeltaTime, float speed, float& velocity, float input, bool bAccCondition);

	float GetRotVelocity(float DeltaTime, float currentVel,float input, bool bAccCondition);

	UPROPERTY(EditDefaultsOnly)
	float maxSpeed = 500.0f;


	UPROPERTY(BlueprintReadOnly)
	float moveVelocity = .0f;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float velDecelerationSpeed = 0.2f;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float velAccelerationSpeed = 0.3f;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "0.99"))
	float turnDecelerationSpeed = 0.01f;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float turnAccelerationSpeed = 0.8f;

	UPROPERTY(EditDefaultsOnly)
		float maxMeshRoll = 15.0f;

	float throttle;
	float turnDirection;
	float collTurnAdditive = 0.f;

	bool bAllowGearChange = false;
	bool bOnChangedGears = false;
	TArray<float> throttleGears = { -0.45f,0.f,0.45f,0.7f,1.f };
	int gearIndex = 1;
	int gearClamp = 4;

	int crewCount = 0;

	UPROPERTY(BlueprintReadOnly)
		float turnVelocity = .0f;

	bool bUpdateTurnVel = false;

	class ABoatPawn* owner;
	class UStaticMeshComponent* body;
	class UCapsuleComponent* pushCollider;
	class USceneComponent* mesh;

public:	

	void UpdateBoatRotation(float DeltaTime);

	void UpdateMeshHolderRoll();

	void UpdateBoatMovement(float DeltaTime);
};
