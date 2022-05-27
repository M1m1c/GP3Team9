
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GP3Enemy.generated.h"

UCLASS()
class GP3TEAM9_API AGP3Enemy : public AActor
{
	GENERATED_BODY()

public:
	AGP3Enemy();
	void Cruising();
	void ChasePlayer();
	void AttackPlayer();
	void CheckLineOfSightToPlayer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	
	enum class EAIState { Cruising, Chasing, Attacking  };
	EAIState CurrentState = EAIState::Cruising;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APawn* TargetPlayer = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxAttackDistance = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FHitResult HitResult;
};
