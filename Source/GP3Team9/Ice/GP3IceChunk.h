
#pragma once

#include "CoreMinimal.h"
#include "GP3Team9/Boat/FloatComp.h"
#include "GP3Team9/Boat/FloatingProp.h"
#include "GP3IceChunk.generated.h"

UCLASS(Abstract, Blueprintable)
class GP3TEAM9_API AGP3IceChunk : public AFloatingProp
{
	GENERATED_BODY()

public:
	AGP3IceChunk();

	virtual void OnOverlapBegin(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult) override;

	virtual void Tick(float DeltaTime) override;

	FVector GetRandomFloaterPos();

	void ReplaceMesh(UStaticMesh* NewMesh, FVector offset,FVector size);
	void ReplaceMaterial(UMaterialInterface* NewMaterial);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UStaticMeshComponent* MeshComponent;

protected:
	float defaultAngularDamping = 0.f;
	float defaultLinearDamping = 0.f;
	float previousDeltaTime = 0.f;

	float currentTime = 0.f;
	float lifeTime = 4.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFloatComp* FloatComp1;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFloatComp* FloatComp2;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UFloatComp* FloatComp3;

	
};
