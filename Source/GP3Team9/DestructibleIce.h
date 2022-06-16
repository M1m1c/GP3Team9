
#pragma once


#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/BoxComponent.h"
#include "DestructibleIce.generated.h"

UCLASS()
class GP3TEAM9_API ADestructibleIce : public AActor
{
	GENERATED_BODY()

public:
	ADestructibleIce();

	UFUNCTION()
	void Damage(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const
	            FHitResult& Hit);

	UFUNCTION()
	void Trigger(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool
	             bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void Destroy();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Destructible)
	UBoxComponent* TriggerComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Destructible)
	class UDestructibleComponent* DestructibleComponent;

	
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
