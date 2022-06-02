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
	void ChasePlayer();
	void AttackPlayer();
	FVector GetRandomizedFiringTargetPosition();
	void CheckLineOfSightToPlayer();
	bool BHasLineOfSight();
	bool BIsDirectionClear(FVector Direction);

	void GoToLastKnownPosition();
	void SteerTowardsLocation(FVector TargetLocation);
	virtual void Tick(float DeltaSeconds) override;
	void Logger (FString input);
	FString LastLog = "";


protected:
	virtual void BeginPlay() override;

	
public:
	enum class EAIState { Cruising, Chasing, GoingToLastKnownPosition, Attacking  };
	EAIState CurrentState = EAIState::Cruising;


	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* TargetPlayer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAttackDistance = 5000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinDistanceToPlayer = 3000;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult HitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector LastKnownPlayerPosition;

	
	
};






