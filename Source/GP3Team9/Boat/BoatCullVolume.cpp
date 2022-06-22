
#include "BoatCullVolume.h"

#include "BoatPawn.h"
#include "FloatDisablerSphere.h"
#include "GunDriverComp.h"
#include "ShipGun.h"
#include "CrewPickup.h"

#include "Components/BoxComponent.h"
#include "DrawDebugHelpers.h"

ABoatCullVolume::ABoatCullVolume()
{
	PrimaryActorTick.bCanEverTick = false;
	boxVolume = CreateDefaultSubobject<UBoxComponent>(FName("collider"));
	SetRootComponent(boxVolume);
	boxVolume->OnComponentBeginOverlap.AddDynamic(this, &ABoatCullVolume::OnOverlapBegin);
	boxVolume->OnComponentEndOverlap.AddDynamic(this, &ABoatCullVolume::OnOverlapEnd);
	boxVolume->bShouldCollideWhenPlacing = true;
}

void ABoatCullVolume::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!OtherActor) { return; }
	auto boat = Cast<ABoatPawn>(OtherActor);
	if (!boat) { return; }

	auto bNotPlayerBoat = !Cast<APlayerController>(boat->GetController());
	if (bNotPlayerBoat)
	{
		if (!collectedActors.Contains(OtherActor))
		{
			collectedActors.Add(OtherActor);
		}
		return;
	}

	bIsPlayerInVolume = true;
	SetCollectedActorsActive(true);
}

void ABoatCullVolume::OnOverlapEnd(
	class UPrimitiveComponent* OverlappedComp,
	class AActor* OtherActor,
	class UPrimitiveComponent* OtherComp,
	int32 OtherBodyInd)
{
	if (!OtherActor) { return; }

	auto boat = Cast<ABoatPawn>(OtherActor);
	if (!boat) { return; }

	if (!boat->IsHidden() && collectedActors.Contains(OtherActor)) { collectedActors.Remove(OtherActor); }

	auto player = Cast<APlayerController>(boat->GetController());
	if (!player) { return; }

	bIsPlayerInVolume = false;
	SetCollectedActorsActive(false);
}

void ABoatCullVolume::BeginPlay()
{
	Super::BeginPlay();
	GetWorldTimerManager().SetTimer(initialiseCollectOverlapingActorsTimer, this, &ABoatCullVolume::Initalize, 0.1f, false, 0.1f);

}

void ABoatCullVolume::Initalize()
{
	GetWorldTimerManager().ClearTimer(initialiseCollectOverlapingActorsTimer);
	TraceCollectActorsToCull();
	SetCollectedActorsActive(false);
	bInitalized = true;

}

void ABoatCullVolume::TraceCollectActorsToCull()
{
	TArray<FHitResult> OutHits;

	FCollisionShape MyColBox = FCollisionShape::MakeBox(boxVolume->GetScaledBoxExtent());
	auto location = GetActorLocation();
	bool isHit = GetWorld()->SweepMultiByChannel(OutHits, location, location, FQuat(GetActorRotation()), ECC_GameTraceChannel2, MyColBox);
	if (!isHit) { return; }
	for (auto hit : OutHits)
	{
		auto actor = hit.GetActor();
		if (!actor) { continue; }

		if (Cast<AFloatDisablerSphere>(actor)) { continue; }
		if (Cast<AShipGun>(actor)) { continue; }
		if (Cast<ACrewPickup>(actor)) { continue; }

		auto cullVolume = Cast<ABoatCullVolume>(actor);
		if (cullVolume)
		{
			if (cullVolume != this)
			{
				overlappingVolumes.Add(cullVolume);
			}
			continue;
		}

		auto boat = Cast<ABoatPawn>(actor);
		if (boat)
		{
			auto controller = boat->GetController();
			auto bIsPlayerControlled = controller && Cast<APlayerController>(controller);
			if (bIsPlayerControlled) { continue; }
		}

		if (!Cast<AFloatingProp>(actor) || !boat) { continue; }

		collectedActors.Add(actor);
	}
}

void ABoatCullVolume::SetCollectedActorsActive(bool bEnable)
{
	if (!collectedActors.Num()) { return; }
	if (!(collectedActors.Num() > 0)) { return; }
	for (int i = collectedActors.Num() - 1; i >= 0; i--)
	{
		if (!collectedActors.IsValidIndex(i)) { continue; }
		auto actor = collectedActors[i];

		if (actor == nullptr || actor->IsActorBeingDestroyed())
		{
			collectedActors.RemoveAt(i);
			continue;
		}

		SetSingleActorActive(actor, bEnable);

		if (bEnable == false && bInitalized)
		{
			for (auto volume : overlappingVolumes)
			{
				volume->ReEnableOverlappingActor(actor);
			}
		}
	}
}

void ABoatCullVolume::SetSingleActorActive(AActor* actor, bool bEnable)
{
	if (!actor) { return; }
	if (actor->IsActorBeingDestroyed()) { return; }
	if (actor->IsPendingKillOrUnreachable()) { return; }
	actor->SetActorHiddenInGame(!bEnable);
	actor->SetActorEnableCollision(bEnable);
	actor->SetActorTickEnabled(bEnable);

	auto boat = Cast<ABoatPawn>(actor);
	if (boat)
	{
		auto guns = boat->gunDriverComp->GetAllGuns();
		for (auto gun : guns)
		{
			SetSingleActorActive(gun, bEnable);
			auto cast = Cast<AShipGun>(gun);
			if (cast) {
				cast->bCanFire = bEnable;
			}
		}
	}
}

void ABoatCullVolume::ReEnableOverlappingActor(AActor* disabledActor)
{
	if (!bIsPlayerInVolume) { return; }
	if (!disabledActor) { return; }
	if (!collectedActors.Contains(disabledActor)) { return; }
	if (disabledActor->IsActorTickEnabled()) { return; }
	SetSingleActorActive(disabledActor, true);
}
