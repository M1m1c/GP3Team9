
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BoatCullVolume.generated.h"

//TODO these need to ahve a delegate that can send out an actor pointer.
//whenever we disable an actor we need to call the delegate.
//Every overlapping neighbouring boat cull volume should be subscribed to this delegate.
//if the actor pointer exists in a neighbour that the player is in then said neighbour reenables it.

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDisableActor, AActor*, disabledActor);

UCLASS(Abstract, Blueprintable)
class GP3TEAM9_API ABoatCullVolume : public AActor
{
	GENERATED_BODY()
public:

	//FDisableActor OnDisableActor;

	ABoatCullVolume();

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

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere)
		class UBoxComponent* boxVolume;

	void Initalize();

	void TraceCollectActorsToCull();

	void SetCollectedActorsActive(bool bEnable);

	void SetSingleActorActive(AActor* actor, bool bEnable);

	FTimerHandle initialiseCollectOverlapingActorsTimer;

	TArray<AActor*> collectedActors;
	TArray<ABoatCullVolume*> overlappingVolumes;

	UFUNCTION()
	void ReEnableOverlappingActor(AActor* disabledActor);
	
	bool bIsPlayerInVolume = false;
	bool bInitalized = false;
};
