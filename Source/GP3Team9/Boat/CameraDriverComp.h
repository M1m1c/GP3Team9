
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CameraDriverComp.generated.h"


UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GP3TEAM9_API UCameraDriverComp : public UActorComponent
{
	GENERATED_BODY()

public:
	UCameraDriverComp();

	void Initalize(class USceneComponent* holder, class USpringArmComponent* arm);

	bool Initalized = false;

	void ReadCameraHorizontal(float value);

	void ReadCameraVertical(float value);

	void ReadCameraZoom(float value);

	void AimLeft();

	void StopAimLeft();

	void AimRight();

	void StopAimRight();

	void ToggleCameraFreeLook();

	bool GetbFreeCameraLook();
protected:

	void UpdateCameraRotation(float DeltaTime);

	void UpdateCameraZoom(float DeltaTime);

	void UpdateResetCamera(float DeltaTime);

	UPROPERTY(EditDefaultsOnly)
		float minCameraZoom = 100.f;
	UPROPERTY(EditDefaultsOnly)
		float maxCameraZoom = 2500.f;

	class USceneComponent* cameraHolder;
	class USpringArmComponent* cameraArm;
	class ABoatPawn* owner;

	FTransform cameraHolderDefaultTransform;
	FTransform cameraArmDefaultTransform;
	float CameraRotationSpeed = 100.f;
	float CameraHorizontal = 0.f;
	float CameraVertical = 0.f;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "3.0", UIMin = "0.0", UIMax = "3.0"))
	float CameraResetStartTimerMax = 0.1f;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float CameraResetSpeed = 0.2f;

	float CameraResetStartTimer = 0.f;
	float CameraResetTime = 0.f;
	

	float zoomDirection = 0;
	float zoomVelocity = 0;
	UPROPERTY(EditDefaultsOnly)
	float zoomSpeed = 700.f;
	float zoomDecelerationSpeed = 1.f;
	float zoomAccelerationSpeed = 800.f;
	bool bZoomAccOrDec = false;

	bool bFreeCameraLook = true;
	bool bShouldResetCamera = false;
	bool bNoHorizontalCamInput = false;
	bool bNoVerticalCamInput = false;

	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float aimingStartTimerMax = 0.3f;
	UPROPERTY(EditDefaultsOnly, meta = (ClampMin = "0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
	float aimingSpeed = 0.3f;

	float aimingStartTimer=0.f;
	float aimingTime=0.f;

	bool bIsAimingLeft = false;
	bool bIsAimingRight = false;
	float aimingDegrees = 80.f;
	FRotator aimingRotation;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void UpdateAiming(float DeltaTime);

};
