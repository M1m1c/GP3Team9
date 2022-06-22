
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DamagableSystem.h"
#include "BoatMovementComp.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GP3TEAM9_API UBoatMovementComp : public UActorComponent, public IDamagableSystem
{
	GENERATED_BODY()

public:

	void Initalize();

	void AllowGearChange();
	void DisallowGearChange();

	void ReadGearChange(float value);

	void SetGear(int newGear);
	int GetGear();

	void ReadTurning(float value);
	void IncreaseCollTurnAdditive(float valueToAdd);

	void UpdateBoatRotation(float DeltaTime);

	void UpdateMeshHolderRoll();

	void UpdateBoatMovement(float DeltaTime);

	void AddPushForce(FVector forceVector);

	FHitResult AttemptMoveBoat(FVector& velocity, FVector& accumuliativePush, bool bIgnoreSweep);

	int GetBoatSize();

	virtual EHealthSectionPosition GetSectionPosition() override;
	virtual void DisableSystem() override;
	virtual void EnableSystem() override;
	virtual void UpdateCrewCount(int newCrewCount) override;

	UPROPERTY(EditDefaultsOnly)
		float turnSpeed = 80.f;

	bool bInitalized = false;

protected:

	void VeerAwayFromObstacle(FHitResult& hit, FVector& ownerLocation, float DeltaTime);

	void PushOtherBoat(class ABoatPawn* otherBoat, FVector& ownerLocation, float force, float DeltaTime);

	float GetUpdatedRotAxis(float DeltaTime, float speed, float& velocity, float input, bool bAccCondition);

	float GetRotVelocity(float DeltaTime, float currentVel, float input, bool bAccCondition);

	void UpdateGearClamp();

	void CollideWithObstacle();

	float AvoidClippingHitObstalce(
		FHitResult& hit,
		FVector& normal2D,
		FVector& velocity,
		FVector& ownerLocation,
		float DeltaTime,
		float RemainingTime);

	bool IgnoreIceCollisions(FHitResult& hit,
		FVector& velocity,
		FVector& accumuliativePush,
		int& Iterations);

	FVector CalculateAccumulativePushForce();

	UPROPERTY(EditDefaultsOnly)
		float maxSpeed = 500.0f;

	UPROPERTY(BlueprintReadOnly)
		float moveVelocity = .0f;

	UPROPERTY(BlueprintReadOnly)
		float turnVelocity = .0f;

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

	UPROPERTY(EditDefaultsOnly)
		// 1-3. 1 small, 3 big.
		int boatSize = 1;

	float throttle;
	float turnDirection;
	float collTurnAdditive = 0.f;

	TArray<float> throttleGears = { -0.45f,0.f,0.45f,0.7f,1.f };
	int gearIndex = 1;
	int gearClamp = 4;
	int crewCount = 0;

	bool bDisabled = false;
	bool bAllowGearChange = true;
	bool bOnChangedGears = false;
	bool bUpdateTurnVel = false;

	class ABoatPawn* owner;
	class UStaticMeshComponent* body;
	class UCapsuleComponent* pushCollider;
	class USceneComponent* mesh;
	class UIceBreakerComp* iceBreakerComp;

	UPROPERTY()
	TArray<FVector> pushForces;
};
