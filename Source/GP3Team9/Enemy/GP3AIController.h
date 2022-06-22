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


	
	UPROPERTY()
	APlayerController* PlayerController = nullptr;

	UPROPERTY(BlueprintReadWrite)
	AActor* TargetPlayer = nullptr;

	UPROPERTY()
	FVector StartPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//Enemy will turn around if he gets this far from his starting point.
	float PerimeterRadius = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//Inside this distance, enemy will shoot.
	float MaxAttackDistance = 5000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//Outside this range enemy will not "see" player.
	float MaxChasingDistance = 15000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//Enemy will not go closer to player than this.
	float MinDistanceToPlayer = 3000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//In degrees.
	float SideGunAngleRange = 40;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//Enemy will fire towards the player at a random spot within a square with these sides.
	float TargetBoxSize = 700;
	
	UPROPERTY()
	FHitResult HitResult;

	UPROPERTY()
	FVector LastKnownPlayerPosition;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// 2 + small random secs. Keep in mind, gun component also has a delay. 
	float SwivelGunFireDelay;
	float SwivelGunTimeSinceLastFire = 0;
};






