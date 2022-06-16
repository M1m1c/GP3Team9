
#pragma once

#include "CoreMinimal.h"
#include "GP3Team9/Boat/FloatingProp.h"
#include "GP3IceSlab.generated.h"

UCLASS()
class GP3TEAM9_API AGP3IceSlab : public AFloatingProp
{
	GENERATED_BODY()

public:
	AGP3IceSlab();

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<AFloatingProp> IceChunkClass;

	virtual void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit) override;	
	
protected:
	virtual void BeginPlay() override;

	FRotator GetRandomRotation();
};
