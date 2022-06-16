
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Floatable.h"
#include "BoatPawn.generated.h"

UCLASS(Abstract, Blueprintable)
class GP3TEAM9_API ABoatPawn : public APawn, public IFloatable
{
	GENERATED_BODY()

public:
	ABoatPawn();

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* root;

	UPROPERTY(VisibleAnywhere)
		class UCapsuleComponent* pushCollider;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* boatBody;

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* meshHolder;

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* meshOffset;

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* graphicsHolder;

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* ArmorTier1Graphics;

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* ArmorTier2Graphics;

	UPROPERTY(VisibleAnywhere)
		class USceneComponent* ArmorTier3Graphics;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* IceBreakerMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USceneComponent* cameraHolder;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USpringArmComponent* cameraArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCameraComponent* cameraComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCameraDriverComp* cameraDriver;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCameraShakeComp* cameraShaker;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UBoatMovementComp* boatMovementComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UGunDriverComp* gunDriverComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UFloatMasterComp* floatMasterComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UHealthComp* healthComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UCrewComp* crewComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UIceBreakerComp* iceBreakerComp;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		//class UUpgradeManagerComp* upgradeComp;

protected:
	virtual void BeginPlay() override;

	void UpdateActorZPosition(float DeltaTime);

	UFUNCTION()
		void OnBoatDeath();

	UFUNCTION()
		void OnSendCrew(int startSection, int endSection);

	UFUNCTION()
		void OnSectionDestroyed(UHealthSection* sectionAffected);

	UFUNCTION()
		void OnSectionRepaired(UHealthSection* sectionAffected);

	UFUNCTION()
	void OnSectionHit();

	bool bIsBoatDead = false;

	float defaultLinearDamping;
	float defaultAngularDamping;
	float previousDeltaTime = 0.0f;

	float collisionTimer = 0.f;
	float maxCollisionTimer = 1.5f;

public:
	virtual void Tick(float DeltaTime) override;

	bool GetIsInCameraMode();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintImplementableEvent)
		void OnGearChange(int newThrottleGear);

	UFUNCTION(BlueprintImplementableEvent)
		void OnSendCrewToSection(int startSection, int endSection);

	UFUNCTION(BlueprintImplementableEvent)
		void OnNewCrewAddedToReserve();

	UFUNCTION(BlueprintImplementableEvent)
		void OnSectionDestroyedEvent(UHealthSection* sectionAffected);

	UFUNCTION(BlueprintImplementableEvent)
		void OnSectionRepairedEvent(UHealthSection* sectionAffected);

	UFUNCTION(BlueprintImplementableEvent)
		void OnSectionHitEvent();

	UFUNCTION(BlueprintImplementableEvent)
		void OnBoatDeathEvent();

	UFUNCTION(BlueprintImplementableEvent)
		void OnBoatSwapEvent(AActor* newBoat);

	void ApplyCollisionDamage(float damage);
	void ApplyCollisionShake(float trauma);

	float GetLinearVelocityChange(float deltaTime, float accelSpeed);

	float GetProportionalVelocityChange(float deltaTime, float currentVelocity, float accelSpeed, float decelSpeed, bool changeCondition);
	float GetPropVelocityChangeConstantDec(float deltaTime, float currentVelocity, float accelSpeed, float decelSpeed, bool changeCondition);

	virtual UStaticMeshComponent* GetFloatBody() override;


	void SwapBoat(TSubclassOf<ABoatPawn> boatToSwapTo);

	UFUNCTION(BlueprintCallable)
		void DestroyBoat();

	UFUNCTION(BlueprintCallable)
		TMap<EGunSlotPosition, class AShipGun*> GetShipGuns();

	void LevelUpBoat();
	UPROPERTY(EditDefaultsOnly)
		TSubclassOf<ABoatPawn> NextLevelBoat;

	UPROPERTY(EditAnywhere)
		TSubclassOf<class AFloatingProp> itemToDropOnDeath;

	/*UPROPERTY(EditDefaultsOnly)
		TSubclassOf<class UArmorUpgrade> boatUpgrade;
	void UpgradeTest();*/
};
