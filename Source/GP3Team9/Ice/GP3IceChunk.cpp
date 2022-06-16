

#include "GP3IceChunk.h"



AGP3IceChunk::AGP3IceChunk()
{
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComp");
	MeshComponent->SetupAttachment(physicsBody);
	
	FloatComp1 = CreateDefaultSubobject<UFloatComp>(TEXT("FloatComp1"));
	FloatComp1->SetupAttachment(physicsBody);
	FloatComp2 = CreateDefaultSubobject<UFloatComp>(TEXT("FloatComp2"));
	FloatComp2->SetupAttachment(physicsBody);
	FloatComp3 = CreateDefaultSubobject<UFloatComp>(TEXT("FloatComp3"));
	FloatComp3->SetupAttachment(physicsBody);
		
	FloatComp1->SetRelativeLocation(GetRandomFloaterPos());
	FloatComp2->SetRelativeLocation(GetRandomFloaterPos());
	FloatComp3->SetRelativeLocation(GetRandomFloaterPos());

	
}


void AGP3IceChunk::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// FVector NormalIsh = (OverlappedComp->GetOwner()->GetActorLocation() - OtherActor->GetActorLocation().Normalize());
	// physicsBody->AddImpulseAtLocation(NormalIsh * -0.05f, NormalIsh * 0.5f);
	
}

void AGP3IceChunk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	currentTime += DeltaTime;
	if (currentTime > lifeTime) 
	{
		Destroy();
	}
}


FVector AGP3IceChunk::GetRandomFloaterPos()
{
	float Random = FMath::RandRange(-50, 50);
	return FVector(Random, Random, 0);
}

void AGP3IceChunk::ReplaceMesh(UStaticMesh* NewMesh,FVector offset, FVector size)
{
	MeshComponent->SetStaticMesh(NewMesh);
	MeshComponent->SetWorldLocation(offset);
	MeshComponent->SetWorldScale3D(size);
}

void AGP3IceChunk::ReplaceMaterial(UMaterialInterface* NewMaterial)
{
	MeshComponent->SetMaterial(0, NewMaterial);
}




