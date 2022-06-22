
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoatEnums.h"
#include "GunDriverComp.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GP3TEAM9_API UGunDriverComp : public UActorComponent
{
	GENERATED_BODY()

public:
	UGunDriverComp();

	void SetEnable(bool bEnable);

	UFUNCTION(BlueprintCallable)
	void SetTarget(AController* newTarget);

	void AimLeftGuns();
	void AimRightGuns();
	void FirePortGunsThatAreAiming();
	void FireLeftGuns();
	void FireRightGuns();
	void FireSwivelGun();


	void StopFirePortGuns();
	void StopFireLeftGuns();
	void StopFireRightGuns();
	void StopFireSwivelGun();

	void FireLeftGunsAtLocation(FVector location);

	void FireRightGunsAtLocation(FVector location);

	void FireSwivelGunAtLocation(FVector location);

	TArray<class IDamagableSystem*> GetAllGunSystems();
	TArray<AActor*> GetAllGuns();

	TMap<EGunSlotPosition, class AShipGun*> GetEachDirectionShipGuns();

	void Initalize(class USceneComponent* camHolder);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;



	void DestroyGuns();

protected:

	virtual void BeginPlay() override;

	void SpawnPortGun(class UPortGunSlot* slot, TArray<class AShipGun*>& shipGunList);

	void UpdatePortGunRotation(bool condition, float DeltaTime, float gunAngle, float defaultYaw, TArray<class UPortGunSlot*> slots);

	void UpdateShowAimingIndicator(bool condition, TArray<AShipGun*> guns);

	void FireArrayOfGuns(TArray<AShipGun*> guns, float traumaToAdd);

	void FireSingleShipGun(AShipGun* gun, float traumaToAdd);

	void RotateGunsToFaceTargetController();

	void RotateGunsToLookWithCamera(float DeltaTime);

	class UCameraShakeComp* cameraShaker;

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
	TArray<AActor*> allGuns;
	TMap<EGunSlotPosition,class AShipGun*> eachDirectionShipGuns;

	AController* target = nullptr;
	bool initalized = false;

	bool bFiringSwivel = false;
	bool bFiringLeft = false;
	bool bFiringRight = false;
	bool bEnabled = true;
};
