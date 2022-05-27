
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GunDriverComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3TEAM9_API UGunDriverComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGunDriverComp();

	void FireLeftGuns();
	void FireRightGuns();
	void FireSwivelGun();

	void Initalize(class USceneComponent* camHolder);

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	
	virtual void BeginPlay() override;

	void SpawnPortGun(class UPortGunSlot* slot, TArray<class AShipGun*>& shipGunList);

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

	class UPortGunSlot* swivelGunSlot;
	TArray<class UPortGunSlot*> leftGunSlots;
	TArray<class UPortGunSlot*> rightGunSlots;

	class AShipGun* swivelShipGun;
	TArray<class AShipGun*> leftShipGuns;
	TArray<class AShipGun*> rightShipGuns;

	bool initalized = false;
};
