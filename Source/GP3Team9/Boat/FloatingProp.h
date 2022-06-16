
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Floatable.h"
#include "FloatingProp.generated.h"

UCLASS()
class GP3TEAM9_API AFloatingProp : public AActor, public IFloatable
{
	GENERATED_BODY()
	
public:	
	AFloatingProp();

	UFUNCTION()
		virtual void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
			UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit);
	
	virtual UStaticMeshComponent* GetFloatBody() override;

	UPROPERTY(VisibleAnywhere)
		class USphereComponent* collider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* physicsBody;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UFloatMasterComp* floatMasterComp;

	void SetMesh(class UStaticMesh* meshToSet);

protected:
	virtual void BeginPlay() override;

	bool bUsed = false;
	float defaultAngularDamping = 0.f;
	float defaultLinearDamping = 0.f;
	float previousDeltaTime = 0.f;
public:	
	virtual void Tick(float DeltaTime) override;

};


