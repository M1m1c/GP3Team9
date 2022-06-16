

#include "CameraDriverComp.h"

#include "BoatPawn.h"

#include <Runtime/Engine/Classes/GameFramework/SpringArmComponent.h>
#include <Runtime/Engine/Classes/Camera/CameraComponent.h>

UCameraDriverComp::UCameraDriverComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCameraDriverComp::Initalize(USceneComponent* holder, USpringArmComponent* arm)
{
	if (!ensure(holder)) { return; }
	if (!ensure(arm)) { return; }

	cameraHolder = holder;
	cameraArm = arm;
	cameraHolderDefaultTransform = cameraHolder->GetRelativeTransform();
	cameraArmDefaultTransform = cameraArm->GetRelativeTransform();

	owner = Cast<ABoatPawn>(GetOwner());
	if (!ensure(owner)) { return; }

	bInitalized = true;
}


// Called every frame
void UCameraDriverComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bInitalized) { return; }

	UpdateAiming(DeltaTime);

	if ((CameraResetTime == 1.f && !bFreeCameraLook) ||
		((bIsAimingLeft || bIsAimingRight) && FMath::IsNearlyZero(aimingStartTimer)) ) 
	{ return; }

	UpdateCameraRotation(DeltaTime);
	//UpdateCameraZoom(DeltaTime);
	
	UpdateResetCamera(DeltaTime);
}

void UCameraDriverComp::UpdateAiming(float DeltaTime)
{
	if (bIsAimingLeft || bIsAimingRight)
	{
		aimingStartTimer = FMath::Clamp(aimingStartTimer - DeltaTime, 0.f, aimingStartTimerMax);
	}
	else
	{
		return;
	}

	if (FMath::IsNearlyZero(aimingStartTimer))
	{
		auto currentArmRot = cameraArm->GetRelativeRotation();
		cameraArm->SetRelativeRotation(
			FQuat::Slerp(FQuat(currentArmRot),
				cameraArmDefaultTransform.GetRotation(),
				aimingTime));

		auto currentHolderRot = cameraHolder->GetRelativeRotation();
		cameraHolder->SetRelativeRotation(
			FQuat::Slerp(FQuat(currentHolderRot),
				FQuat(aimingRotation),
				aimingTime));

		aimingTime = FMath::Clamp(aimingTime + (DeltaTime * aimingSpeed), 0.f, 1.f);
	}
}

void UCameraDriverComp::UpdateResetCamera(float DeltaTime)
{
	if (bFreeCameraLook || bIsAimingLeft || bIsAimingRight) { return; }
	bShouldResetCamera = bNoHorizontalCamInput && bNoVerticalCamInput && !bFreeCameraLook;

	if (bShouldResetCamera)
	{
		CameraResetStartTimer = FMath::Clamp(CameraResetStartTimer - DeltaTime, 0.f, CameraResetStartTimerMax);
	}

	if (FMath::IsNearlyZero(CameraResetStartTimer))
	{
		auto currentArmRot = cameraArm->GetRelativeRotation();
		cameraArm->SetRelativeRotation(
			FQuat::Slerp(FQuat(currentArmRot),
				cameraArmDefaultTransform.GetRotation(),
				CameraResetTime));

		auto currentHolderRot = cameraHolder->GetRelativeRotation();
		cameraHolder->SetRelativeRotation(
			FQuat::Slerp(FQuat(currentHolderRot),
				cameraHolderDefaultTransform.GetRotation(),
				CameraResetTime));

		CameraResetTime = FMath::Clamp(CameraResetTime + (DeltaTime * CameraResetSpeed), 0.f, 1.f);
	}
}

void UCameraDriverComp::UpdateCameraRotation(float DeltaTime)
{
	float VerticalInput = CameraVertical;
	if (cameraArm->GetComponentRotation().Pitch < -50)
	{
		VerticalInput = FMath::Clamp(VerticalInput, 0.0f, 1.0f);
	}
	if (cameraArm->GetComponentRotation().Pitch > -5)
	{
		VerticalInput = FMath::Clamp(VerticalInput, -1.0f, 0.0f);
	}
	
	FRotator newArmRotation = FRotator(VerticalInput * CameraRotationSpeed, 0.f, 0.f) * DeltaTime;
	FQuat quatArmRotation = FQuat(newArmRotation);
	cameraArm->AddRelativeRotation(quatArmRotation);

	FRotator newHolderRotation = FRotator(0.f, CameraHorizontal * CameraRotationSpeed, 0.f) * DeltaTime;
	FQuat quatHolderRotation = FQuat(newHolderRotation);
	cameraHolder->AddRelativeRotation(quatHolderRotation);

}

void UCameraDriverComp::UpdateCameraZoom(float DeltaTime)
{
	auto velocityChange = owner->GetPropVelocityChangeConstantDec(
		DeltaTime,
		zoomVelocity,
		zoomAccelerationSpeed,
		zoomDecelerationSpeed,
		bZoomAccOrDec);

	zoomVelocity = FMath::Clamp(zoomVelocity + velocityChange, 0.f, 1.f);
	auto zoomChange = (zoomSpeed * zoomVelocity) * DeltaTime;
	auto newArmLength = FMath::Clamp(cameraArm->TargetArmLength - zoomDirection * zoomChange, minCameraZoom, maxCameraZoom);
	cameraArm->TargetArmLength = newArmLength;
}


void UCameraDriverComp::ReadCameraHorizontal(float value)
{
	bNoHorizontalCamInput = FMath::IsNearlyZero(value);
	CameraResetStartTimer = bNoHorizontalCamInput ? CameraResetStartTimer : CameraResetStartTimerMax;
	CameraResetTime = bNoHorizontalCamInput ? CameraResetTime : 0.f;
	bFreeCameraLook = bNoHorizontalCamInput ? bFreeCameraLook : true;

	aimingStartTimer = bNoHorizontalCamInput ? aimingStartTimer : aimingStartTimerMax;
	aimingTime = bNoHorizontalCamInput ? aimingTime : 0.f;

	CameraHorizontal = value;
}

void UCameraDriverComp::ReadCameraVertical(float value)
{
	bNoVerticalCamInput = FMath::IsNearlyZero(value);
	CameraResetStartTimer = bNoVerticalCamInput ? CameraResetStartTimer : CameraResetStartTimerMax;
	CameraResetTime = bNoVerticalCamInput ? CameraResetTime : 0.f;
	bFreeCameraLook = bNoVerticalCamInput ? bFreeCameraLook : true;

	aimingStartTimer = bNoVerticalCamInput ? aimingStartTimer : aimingStartTimerMax;
	aimingTime = bNoVerticalCamInput ? aimingTime : 0.f;

	CameraVertical = value;
}

void UCameraDriverComp::ReadCameraZoom(float value)
{
	if (value > 0.f || value < 0.f)
	{
		bZoomAccOrDec = true;
		zoomDirection = value;
	}
	else if (FMath::IsNearlyZero(value))
	{
		bZoomAccOrDec = false;
	}

}

void UCameraDriverComp::AimLeft()
{
	aimingTime = 0.f;
	aimingStartTimer = aimingStartTimerMax;
	bIsAimingLeft = true;
	aimingRotation = FRotator(0.f, -aimingDegrees, 0.f);
}

void UCameraDriverComp::StopAimLeft()
{
	bIsAimingLeft = false;
	CameraResetStartTimer = CameraResetStartTimerMax;
	CameraResetTime = 0.f;
}

void UCameraDriverComp::AimRight()
{
	aimingTime = 0.f;
	aimingStartTimer = aimingStartTimerMax;
	bIsAimingRight = true;
	aimingRotation = FRotator(0.f, aimingDegrees, 0.f);
}

void UCameraDriverComp::StopAimRight()
{
	bIsAimingRight = false;
	CameraResetStartTimer = CameraResetStartTimerMax;
	CameraResetTime = 0.f;
}

void UCameraDriverComp::ToggleCameraFreeLook()
{
	bFreeCameraLook = !bFreeCameraLook;
}

bool UCameraDriverComp::GetbFreeCameraLook()
{
	return bFreeCameraLook;
}

