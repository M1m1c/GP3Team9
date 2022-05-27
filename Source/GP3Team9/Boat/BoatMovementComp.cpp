

#include "BoatMovementComp.h"

#include "BoatPawn.h"

#include "Components/CapsuleComponent.h"

UBoatMovementComp::UBoatMovementComp()
{
}

void UBoatMovementComp::Initalize()
{
	owner = Cast<ABoatPawn>(GetOwner());
	if (!ensure(owner)) { return; }
	body = owner->boatBody;
	if (!ensure(body)) { return; }
	pushCollider = owner->pushCollider;
	if (!ensure(pushCollider)) { return; }
	mesh = owner->meshHolder;
	if (!ensure(mesh)) { return; }

	Initalized = true;
}

void UBoatMovementComp::AllowGearChange()
{
	bAllowGearChange = true;
}

void UBoatMovementComp::DisallowGearChange()
{

	bAllowGearChange = false;
}

void UBoatMovementComp::ReadGearChange(float value)
{
	if (!bAllowGearChange) { return; }
	if (FMath::IsNearlyZero(value))
	{
		bOnChangedGears = false;
		return;
	}
	else if (bOnChangedGears) { return; }

	bOnChangedGears = true;

	auto bOnLastGear = gearIndex == (throttleGears.Num() - 1);
	auto bOnFirstGear = gearIndex == 0;
	if (value > 0.f && !bOnLastGear) {
		gearIndex++;
	}
	else if (value < 0.f && !bOnFirstGear)
	{
		gearIndex--;
	}
	else
	{
		return;
	}
	owner->OnGearChange(gearIndex);
	throttle = throttleGears[gearIndex];
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::White, FString::Printf(TEXT("Gear Changed to: %d"), gearIndex));
}

void UBoatMovementComp::ReadTurning(float value)
{
	if (FMath::IsNearlyZero(value))
	{
		bUpdateTurnVel = false;
		return;
	}
	turnDirection = FMath::Clamp(value, -1.f, 1.f);;
	bUpdateTurnVel = true;
}

void UBoatMovementComp::UpdateBoatMovement(float DeltaTime)
{
	auto newVelocity = owner->GetLinearVelocityChange(
		DeltaTime,
		moveVelocity > throttle ? -velAccelerationSpeed : velAccelerationSpeed);

	auto velocityChange = moveVelocity + newVelocity;
	auto bLargerThanGear = newVelocity > 0.f && velocityChange > throttle;
	auto bLessThanGear = newVelocity < 0.f && velocityChange < throttle;
	moveVelocity = FMath::Clamp(bLargerThanGear || bLessThanGear ? moveVelocity : velocityChange, -1.f, 1.f);

	FVector location = pushCollider->GetComponentLocation();

	float RemainingTime = DeltaTime;
	int Iterations = 0;

	auto force = (maxSpeed * DeltaTime * moveVelocity);
	auto direction = owner->GetActorForwardVector();
	auto velocity = direction * force;
	while (RemainingTime > 0.f && ++Iterations < 10)
	{
		FHitResult hit;

		pushCollider->AddWorldOffset(velocity, true, &hit);

		auto colliderLocation = pushCollider->GetComponentLocation();
		owner->SetActorLocation(FVector(colliderLocation.X, colliderLocation.Y, owner->GetActorLocation().Z));

		auto ownerLocation = owner->GetActorLocation();
		auto bodyLocation = body->GetComponentLocation();

		auto newWorldLocaiton = FVector(
			ownerLocation.X,
			ownerLocation.Y,
			bodyLocation.Z);

		body->SetWorldLocation(newWorldLocaiton);


		if (hit.bBlockingHit)
		{
			auto normal2D = FVector(hit.Normal.X, hit.Normal.Y, 0.f);

			if (hit.bStartPenetrating)
			{
				pushCollider->AddWorldOffset(normal2D * (hit.PenetrationDepth + 0.1f));
			}
			else
			{
				velocity = FVector::VectorPlaneProject(velocity, normal2D);

				if (!FMath::IsNearlyZero(moveVelocity))
				{
					auto hitLocation2D = FVector(hit.Location.X, hit.Location.Y, 0.f);
					auto ownerLocation2D = FVector(ownerLocation.X, ownerLocation.Y, 0.f);

					auto delta = FVector(hitLocation2D - ownerLocation2D).GetSafeNormal();
					auto cross = FVector::CrossProduct(delta, owner->GetActorForwardVector());

					auto dir = (cross.Z < 0.f ? DeltaTime : -DeltaTime) * 3.f;
					collTurnAdditive = FMath::Clamp(collTurnAdditive + dir, -2.f, 2.f);

				}

				RemainingTime -= RemainingTime * hit.Time;
			}
		}
		else
		{
			auto negativeAddClamp = FMath::Clamp(collTurnAdditive + (DeltaTime * 2.f), -2.f, 0.f);
			auto possitiveAddClamp = FMath::Clamp(collTurnAdditive - (DeltaTime * 2.f), 0.f, 2.f);
			collTurnAdditive = collTurnAdditive < 0.f ? negativeAddClamp : possitiveAddClamp;
			break;
		}
	}
}


void UBoatMovementComp::UpdateBoatRotation(float DeltaTime)
{
	auto updatedYaw = GetUpdatedRotAxis(DeltaTime, turnSpeed, turnVelocity, throttle < 0.f ? -turnDirection : turnDirection, bUpdateTurnVel);

	FRotator newRotation = FRotator(0.f, updatedYaw, 0.f) * DeltaTime * FMath::Clamp(FMath::Abs(moveVelocity), 0.2f, 1.f);

	FQuat quatRotation = FQuat(newRotation);

	owner->AddActorLocalRotation(quatRotation, false, 0, ETeleportType::None);

	auto bodyRot = body->GetComponentRotation();
	auto ownerRot = owner->GetActorRotation();
	auto newBodyRotation = FQuat(FRotator(bodyRot.Pitch, ownerRot.Yaw, bodyRot.Roll));

	body->SetWorldRotation(newBodyRotation);

	UpdateMeshHolderRoll();
}

void UBoatMovementComp::UpdateMeshHolderRoll()
{
	auto alphaRoll = FMath::GetMappedRangeValueClamped(FVector2D(-1.f, 1.f), FVector2D(0.f, 1.f), turnVelocity + collTurnAdditive);
	auto angle = maxMeshRoll + FMath::Abs(collTurnAdditive);
	auto lerpRoll = FMath::Lerp(-angle, angle, alphaRoll);
	mesh->SetRelativeRotation(FQuat(FRotator(0.f, 0.f, lerpRoll)));
}

float UBoatMovementComp::GetUpdatedRotAxis(float DeltaTime, float speed, float& velocity, float input, bool bAccCondition)
{
	velocity = GetRotVelocity(DeltaTime, velocity, input, bAccCondition);
	return speed * (velocity + collTurnAdditive);
}

float UBoatMovementComp::GetRotVelocity(float DeltaTime, float currentVel, float input, bool bAccCondition)
{
	auto bNegativeInput = input < 0.f;
	auto bNegativeVelocity = currentVel < 0.f;
	auto turnAcc = bNegativeInput ? -turnAccelerationSpeed : turnAccelerationSpeed;
	auto turnDec = bNegativeVelocity ? turnDecelerationSpeed : -turnDecelerationSpeed;

	auto velocityChange = owner->GetProportionalVelocityChange(
		DeltaTime,
		currentVel,
		turnAcc,
		turnDec,
		bAccCondition);
	auto newVelocity = FMath::Clamp(currentVel + velocityChange, -1.f, 1.f);

	auto possitiveClamp = FMath::Clamp(newVelocity, 0.f, 1.f);
	auto negativeClamp = FMath::Clamp(newVelocity, -1.f, 0.f);
	auto clampedResult = bNegativeVelocity ? negativeClamp : possitiveClamp;

	return bAccCondition ? newVelocity : clampedResult;
}