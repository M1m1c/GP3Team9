#pragma once


#include "CoreMinimal.h"
#include "AIController.h"
#include "../Boat/BoatPawn.h"
#include "Kismet/KismetMaterialLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "GP3AIController.generated.h"


UCLASS(Abstract, Blueprintable)
class GP3TEAM9_API AGP3AIController : public AAIController
{
	GENERATED_BODY()
	AGP3AIController();

	UPROPERTY()
	ABoatPawn* Self;
	
public:
	void Cruise();
	void ChasePlayer();
	void AttackPlayer();
	FVector GetRandomizedFiringTargetPosition();
	void CheckLineOfSightToPlayer();
	bool BPlayerIsWithinSight();
	bool BIsDirectionClear(FVector Direction);

	void GoToLastKnownPosition();
	void SteerTowardsLocation(FVector TargetLocation);
	void TurnAround();
	virtual void Tick(float DeltaSeconds) override;
	void Logger (FString input);
	FString LastLog = "";


protected:
	virtual void BeginPlay() override;
	
public:
	enum class EAIState { Cruising, Chasing, GoingToLastKnownPosition, Attacking  };
	EAIState CurrentState = EAIState::Cruising;


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APlayerController* PlayerController = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetPlayer = nullptr;

	UPROPERTY()
	FVector StartPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float PerimeterRadius = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAttackDistance = 5000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxChasingDistance = 15000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinDistanceToPlayer = 3000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult HitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LastKnownPlayerPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// Delay for Firing. Keep in mind, gun component also has a delay.
	float SwivelGunFireDelay;
	float SwivelGunTimeSinceLastFire = 0;
	
	
};






