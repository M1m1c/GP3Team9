
#pragma once

#include "CoreMinimal.h"
#include "FloatingProp.h"
#include "Seamine.generated.h"

UCLASS(Abstract, Blueprintable)
class GP3TEAM9_API ASeamine : public AFloatingProp
{
	GENERATED_BODY()
	
public:	
	ASeamine();

	UFUNCTION(BlueprintImplementableEvent)
		void OnExplosion();

		virtual void OnOverlapBegin(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult) override;

		UFUNCTION()
		void OnOverlapExit(AActor* self, AActor* OtherActor);
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float explosionDamage = 50.f;

	UPROPERTY(BlueprintReadOnly)
		TArray<class ABoatPawn*> boatsInExplosionRadius;

};
