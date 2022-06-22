

#include "GP3IceSlab.h"
#include "GP3IceChunk.h"
#include "GP3Team9/Boat/BoatPawn.h"
#include "GP3Team9/Boat/IceBreakerComp.h"


AGP3IceSlab::AGP3IceSlab()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AGP3IceSlab::OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnComponentHit(HitComponent, OtherActor, OtherComponent, NormalImpulse, Hit);

	if (!OtherActor) { return; }
	auto boat = Cast<ABoatPawn>(OtherActor);
	if (boat)
	{
		if (!boat->iceBreakerComp->IsIceBreakerComplete()) { return; }

		/*auto Instance = GetWorld()->SpawnActor<AFloatingProp>(
			IceChunkClass,
			HitComponent->GetComponentLocation(),
			HitComponent->GetComponentRotation());

		AGP3IceChunk* IceChunk = Cast<AGP3IceChunk>(Instance);

		auto meshComp = Cast<UStaticMeshComponent>(HitComponent);
		if (meshComp)
		{
			auto mesh = meshComp->GetStaticMesh();
			if (mesh) {
				IceChunk->ReplaceMesh(mesh, GetActorLocation() - FVector(0.f, 0.f, 250.f), HitComponent->GetComponentScale());
			}

			auto Material = HitComponent->GetMaterial(0);
			if (Material)
			{
				IceChunk->ReplaceMaterial(Material);
			}

		}*/
		HitComponent->DestroyComponent();
	}
}

void AGP3IceSlab::BeginPlay()
{
	Super::BeginPlay();

	auto comps = GetComponentsByClass(UStaticMeshComponent::StaticClass());
	for (auto comp : comps)
	{
		auto mesh = Cast<UStaticMeshComponent>(comp);
		if (mesh) {
			mesh->OnComponentHit.AddDynamic(this, &AGP3IceSlab::OnComponentHit);
		}
	}
}

FRotator AGP3IceSlab::GetRandomRotation()
{
	return FRotator(FMath::RandRange(-180, 180));
}
