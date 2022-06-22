
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FloatDisablerSphere.generated.h"


UCLASS()
class GP3TEAM9_API AFloatDisablerSphere : public AActor
{
	GENERATED_BODY()
public:
	AFloatDisablerSphere();

	UFUNCTION()
		virtual void OnOverlapBegin(
			UPrimitiveComponent* OverlappedComp,
			AActor* OtherActor,
			UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex,
			bool bFromSweep,
			const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnOverlapEnd(
			class UPrimitiveComponent* OverlappedComp,
			class AActor* OtherActor,
			class UPrimitiveComponent* OtherComp,
			int32 OtherBodyInd);

	void SetOwner(AActor* newOwner);
	void SetScale(FVector scaleToSet);
protected:

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* sphere;
	AActor* Owner;

	FTimerHandle initialiseSizeTimerHandle;
	FVector sizeToSet = FVector(1.f);
	void UpdateScale();
};
