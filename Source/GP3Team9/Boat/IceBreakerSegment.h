
#pragma once

#include "CoreMinimal.h"
#include "FloatingProp.h"
#include "IceBreakerSegment.generated.h"


UCLASS()
class GP3TEAM9_API AIceBreakerSegment : public AFloatingProp
{
	GENERATED_BODY()
public:
	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;
};
