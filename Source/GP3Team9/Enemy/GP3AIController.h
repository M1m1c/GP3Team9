#pragma once


#include "CoreMinimal.h"
#include "AIController.h"
#include "../Boat/BoatPawn.h"
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
	FVector GetEndPointToCheck(float Angle, float Distance);
	void ChasePlayer();
	void AttackPlayer();
	void CheckLineOfSightToPlayer();
	bool BHasLineOfSight();
	bool BIsPathClear(FVector Direction);

	virtual void Tick(float DeltaSeconds) override;


protected:
	virtual void BeginPlay() override;

	
public:
	enum class EAIState { Cruising, Chasing, Attacking  };
	EAIState CurrentState = EAIState::Cruising;


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetPlayer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAttackDistance = 10000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinDistanceToPlayer = 5000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult HitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LastKnownPlayerLocation;

	
	
};




