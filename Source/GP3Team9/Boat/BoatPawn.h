
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "BoatPawn.generated.h"

UCLASS(Abstract, Blueprintable)
class GP3TEAM9_API ABoatPawn : public APawn
{
	GENERATED_BODY()

public:
	ABoatPawn();

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* root;

	UPROPERTY(VisibleAnywhere)
		class UCapsuleComponent* pushCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* boatBody;

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* meshHolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USceneComponent* cameraHolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* cameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent* cameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCameraDriverComp* cameraDriver;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UBoatMovementComp* boatMovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UGunDriverComp* gunDriverComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UFloatMasterComp* floatMasterComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UHealthComp* healthComp;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCrewComp* crewComp;

protected:
	virtual void BeginPlay() override;

	void UpdateActorZPosition(float DeltaTime);

	UFUNCTION()
	void OnBoatDeath();

	UFUNCTION()
	void OnSendCrew(int startSection, int endSection);

	bool bIsBoatDead = false;

	float defaultLinearDamping;
	float defaultAngularDamping;
	float previousDeltaTime=0.0f;

public:
	virtual void Tick(float DeltaTime) override;

	bool GetIsInCameraMode();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent)
	void OnGearChange(int newThrottleGear);


	UFUNCTION(BlueprintImplementableEvent)
	void OnSendCrewToSection(int startSection, int endSection);

	float GetLinearVelocityChange(float deltaTime, float accelSpeed);

	float GetProportionalVelocityChange(float deltaTime, float currentVelocity, float accelSpeed, float decelSpeed, bool changeCondition);
	float GetPropVelocityChangeConstantDec(float deltaTime, float currentVelocity, float accelSpeed, float decelSpeed, bool changeCondition);
};
