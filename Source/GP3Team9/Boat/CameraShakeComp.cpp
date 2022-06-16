

#include "CameraShakeComp.h"


#include <Runtime/Engine/Classes/Camera/CameraComponent.h>

UCameraShakeComp::UCameraShakeComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCameraShakeComp::Initalize()
{
	auto owner = GetOwner();
	if (!ensure(owner)) { return; }
	camera = Cast<UCameraComponent>(owner->GetComponentByClass(UCameraComponent::StaticClass()));
	if (!ensure(camera)) { return; }
	defaultRotation = camera->GetRelativeRotation();
	bInitalized = true;
}

void UCameraShakeComp::AddShakeTrauma(float traumaToAdd)
{
	Trauma += FMath::Clamp(traumaToAdd, 0.0f, maxTrauma);
}

float UCameraShakeComp::GetTrauma()
{
	return Trauma;
}


void UCameraShakeComp::BeginPlay()
{
	Super::BeginPlay();
}

void UCameraShakeComp::ShakeCamera(float DeltaTime)
{
	Trauma = FMath::Clamp(Trauma - (DeltaTime * DegerdationRateTrauma), 0.0f, maxTrauma);

	auto CameraShake = FMath::Square(Trauma);

	float Pitch = MaxRotation * CameraShake * FMath::RandRange(-1.0f, 1.0f);
	float Yaw = MaxRotation * CameraShake * FMath::RandRange(-1.0f, 1.0f);
	float Roll = MaxRotation * CameraShake * FMath::RandRange(-1.0f, 1.0f);


	FRotator TempRotator = FRotator(Pitch, Yaw, Roll);

	camera->SetRelativeRotation(defaultRotation + TempRotator);
}

void UCameraShakeComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!bInitalized) { return; }
	ShakeCamera(DeltaTime);
}

