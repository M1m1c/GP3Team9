
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GunDriverComp.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GP3TEAM9_API UGunDriverComp : public UActorComponent
{
	GENERATED_BODY()

public:
	UGunDriverComp();

	void SetTarget(AActor* newTarget);

	void AimLeftGuns();
	void AimRightGuns();
	void FireLeftGuns();
	void FireRightGuns();
	void FireSwivelGun();

	void FireLeftGunsAtLocation(FVector location);

	void FireRightGunsAtLocation(FVector location);

	void FireSwivelGunAtLocation(FVector location);

	TArray<class IDamagableSystem*> GetAllGunSystems();

	void Initalize(class USceneComponent* camHolder);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



protected:

	virtual void BeginPlay() override;

	void SpawnPortGun(class UPortGunSlot* slot, TArray<class AShipGun*>& shipGunList);

	void UpdatePortGunRotation(bool condition, float DeltaTime, float gunAngle, float defaultYaw, TArray<class UPortGunSlot*> slots);

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AShipGun> swivelGunBP;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AShipGun> portGunBP;

	class USceneComponent* cameraHolder;
	FRotator swivelDefaultRotation;

	UPROPERTY(EditAnywhere)
		float swivelMinYaw = -240.f;
	UPROPERTY(EditAnywhere)
		float swivelMaxYaw = 60.f;

	UPROPERTY(EditAnywhere)
		float swivelGlobalMinYaw = -150.f;
	UPROPERTY(EditAnywhere)
		float swivelGlobalMaxYaw = 150.f;

	float portGunLeftMaxYaw = -34.f;
	float portGunLeftMinYaw = -148.f;

	float portGunRightMaxYaw = 148.f;
	float portGunRightMinYaw = 34.f;

	float portGunLeftDefaultYaw = 0;
	float portGunRightDefaultYaw = 0;

	float portGunDotAngle = 0.5f;
	float portSlotsLeftNonActiveYaw = -180.f;
	float portSlotsRightNonActiveYaw = 0.f;

	class UPortGunSlot* swivelGunSlot;
	TArray<class UPortGunSlot*> leftGunSlots;
	TArray<class UPortGunSlot*> rightGunSlots;

	class AShipGun* swivelShipGun;
	TArray<class AShipGun*> leftShipGuns;
	TArray<class AShipGun*> rightShipGuns;

	TArray<class IDamagableSystem*> allGunSystems;

	AActor* target = nullptr;
	bool initalized = false;
};
