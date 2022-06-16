
#pragma once

#include "CoreMinimal.h"
#include "FloatingProp.h"
#include "CrewPickup.generated.h"

UCLASS(Abstract, Blueprintable)
class GP3TEAM9_API ACrewPickup : public AFloatingProp
{
	GENERATED_BODY()
	
public:	
	ACrewPickup();

		virtual void OnOverlapBegin(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult) override;

protected:

	UPROPERTY(EditAnywhere)
	int crewToAdd = 1;

};
