

#include "BoatMovementComp.h"

#include "BoatPawn.h"
#include "IceBreakerComp.h"
#include "GP3Team9/Ice/GP3IceSlab.h"
#include "GP3Team9/Ice/GP3IceChunk.h"

#include "Components/CapsuleComponent.h"

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
	iceBreakerComp = owner->iceBreakerComp;
	if (!ensure(iceBreakerComp)) { return; }
	UpdateGearClamp();

	bInitalized = true;
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
	if (FMath::Abs(value) < 0.5f)
	{
		bOnChangedGears = false;
		return;
	}
	else if (bOnChangedGears) { return; }

	bOnChangedGears = true;
	int newGear = gearIndex;
	auto bOnLastGear = gearIndex == gearClamp;
	auto bOnFirstGear = gearIndex == 0;
	if (value > 0.f && !bOnLastGear) {
		newGear++;
	}
	else if (value < 0.f && !bOnFirstGear)
	{
		newGear--;
	}
	else
	{
		return;
	}
	SetGear(newGear);
}

void UBoatMovementComp::SetGear(int newGear)
{

	auto newIndex = FMath::Clamp(newGear, crewCount < 1 ? 1 : 0, gearClamp);
	if (newIndex != gearIndex) { owner->OnGearChange(newIndex); }
	gearIndex = newIndex;
	throttle = throttleGears[gearIndex];
}

int UBoatMovementComp::GetGear()
{
	return gearIndex;
}

void UBoatMovementComp::ReadTurning(float value)
{
	if (!bAllowGearChange) { return; }
	if (FMath::IsNearlyZero(value))
	{
		bUpdateTurnVel = false;
		return;
	}
	turnDirection = FMath::Clamp(value, -1.f, 1.f);;
	bUpdateTurnVel = true;
}

EHealthSectionPosition UBoatMovementComp::GetSectionPosition()
{
	return EHealthSectionPosition::Back;
}

void UBoatMovementComp::DisableSystem()
{
	gearClamp = 2;
	SetGear(gearIndex);
	bDisabled = true;
}

void UBoatMovementComp::EnableSystem()
{
	bDisabled = false;
	UpdateGearClamp();
}

void UBoatMovementComp::UpdateCrewCount(int newCrewCount)
{
	crewCount = newCrewCount;
	UpdateGearClamp();
}

void UBoatMovementComp::UpdateGearClamp()
{
	if (bDisabled)
	{
		if (crewCount < 1)
		{
			gearClamp = 1;
		}
		else if (crewCount < 2)
		{
			gearClamp = 2;
		}
	}
	else
	{
		if (crewCount < 1)
		{
			gearClamp = 1;
		}
		else if (crewCount < 2)
		{
			gearClamp = 2;
		}
		else if (crewCount < 4)
		{
			gearClamp = 3;
		}
		else
		{
			gearClamp = throttleGears.Num() - 1;
		}
	}



	SetGear(gearIndex);
}

void UBoatMovementComp::UpdateBoatMovement(float DeltaTime)
{
	auto newVelocity = owner->GetLinearVelocityChange(
		DeltaTime,
		moveVelocity > throttle ? -velAccelerationSpeed : velAccelerationSpeed);

	auto velocityChange = moveVelocity + newVelocity;
	auto bLargerThanGear = newVelocity > 0.f && velocityChange > throttle;
	auto bLessThanGear = newVelocity < 0.f && velocityChange < throttle;
	auto bNewVelocityExceedingGearThrottle = bLargerThanGear || bLessThanGear;
	moveVelocity = FMath::Clamp(bNewVelocityExceedingGearThrottle ? moveVelocity : velocityChange, -1.f, 1.f);

	FVector location = pushCollider->GetComponentLocation();

	float RemainingTime = DeltaTime;
	int iterations = 0;

	auto force = ((maxSpeed + (crewCount * 3.f)) * DeltaTime * moveVelocity);
	auto direction = owner->GetActorForwardVector();
	auto velocity = direction * force;
	while (RemainingTime > 0.f && ++iterations < 10)
	{


		FVector accumuliativePush = CalculateAccumulativePushForce();
		auto hit = AttemptMoveBoat(velocity, accumuliativePush, false);
		auto ownerLocation = owner->GetActorLocation();


		if (hit.bBlockingHit)
		{
			owner->ApplyCollisionShake(0.5f);

			if (iceBreakerComp->IsIceBreakerComplete())
			{			
				if (IgnoreIceCollisions(hit, velocity, accumuliativePush, iterations))
				{
					continue;
				}	
			}

			auto normal2D = FVector(hit.Normal.X, hit.Normal.Y, 0.f);

			RemainingTime = AvoidClippingHitObstalce(
				hit,
				normal2D, 
				velocity, 
				ownerLocation, 
				DeltaTime, 
				RemainingTime);

			auto other = hit.GetActor();
			if (other && !FMath::IsNearlyZero(moveVelocity))
			{
				auto otherBoat = Cast<ABoatPawn>(other);
				if (otherBoat)
				{
					PushOtherBoat(otherBoat, ownerLocation, force, DeltaTime);
				}
				else
				{
					CollideWithObstacle();
				}
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

void UBoatMovementComp::CollideWithObstacle()
{
	owner->ApplyCollisionDamage(10.f);
	auto bShouldReduceVelocity =
		!FMath::IsNearlyZero(moveVelocity) &&
		(moveVelocity > 0.f && gearIndex > 1);
	if (bShouldReduceVelocity)
	{
		moveVelocity = 0.3f;
	}
}

float UBoatMovementComp::AvoidClippingHitObstalce(FHitResult& hit, FVector& normal2D, FVector& velocity, FVector& ownerLocation, float DeltaTime, float RemainingTime)
{
	if (hit.bStartPenetrating)
	{
		pushCollider->AddWorldOffset(normal2D * (hit.PenetrationDepth + 0.1f));
	}
	else
	{
		velocity = FVector::VectorPlaneProject(velocity, normal2D);

		if (!FMath::IsNearlyZero(moveVelocity))
		{
			VeerAwayFromObstacle(hit, ownerLocation, DeltaTime);
		}

		RemainingTime -= RemainingTime * hit.Time;
	}
	return RemainingTime;
}

bool UBoatMovementComp::IgnoreIceCollisions(FHitResult& hit, FVector& velocity, FVector& accumuliativePush, int& Iterations)
{
	auto retflag = false;
	auto IceChunk = Cast<AGP3IceChunk>(hit.Actor);
	auto IceSlab = Cast<AGP3IceSlab>(hit.Actor);

	if (IceChunk || IceSlab)
	{
		AttemptMoveBoat(velocity, accumuliativePush, true);
		Iterations = 10;
		retflag = true;
	}
	return retflag;
}

FVector UBoatMovementComp::CalculateAccumulativePushForce()
{
	FVector accumuliativePush =FVector::ZeroVector;
	if (pushForces.Num())
	{
		for (int i = pushForces.Num() - 1; i >= 0; i--)
		{
			accumuliativePush += pushForces[i];
			pushForces.RemoveAt(i);
		}
	}
	return accumuliativePush;
}

FHitResult UBoatMovementComp::AttemptMoveBoat(FVector& velocity, FVector& accumuliativePush, bool bIgnoreSweep)
{
	FHitResult hit;
	if (!bIgnoreSweep)
	{
		pushCollider->AddWorldOffset(velocity + accumuliativePush, true, &hit);
	}
	else
	{
		pushCollider->AddWorldOffset(velocity + accumuliativePush);
	}

	auto colliderLocation = pushCollider->GetComponentLocation();
	owner->SetActorLocation(FVector(colliderLocation.X, colliderLocation.Y, owner->GetActorLocation().Z));

	auto ownerLocation = owner->GetActorLocation();
	auto bodyLocation = body->GetComponentLocation();

	auto newWorldLocaiton = FVector(
		ownerLocation.X,
		ownerLocation.Y,
		bodyLocation.Z);

	body->SetWorldLocation(newWorldLocaiton);

	return hit;
}

void UBoatMovementComp::AddPushForce(FVector forceVector)
{
	pushForces.Add(forceVector);
}

int UBoatMovementComp::GetBoatSize()
{
	return boatSize;
}

void UBoatMovementComp::VeerAwayFromObstacle(FHitResult& hit, FVector& ownerLocation, float DeltaTime)
{
	auto hitLocation2D = FVector(hit.Location.X, hit.Location.Y, 0.f);
	auto ownerLocation2D = FVector(ownerLocation.X, ownerLocation.Y, 0.f);

	auto delta = FVector(hitLocation2D - ownerLocation2D).GetSafeNormal();
	auto cross = FVector::CrossProduct(delta, owner->GetActorForwardVector());

	auto dir = (cross.Z < 0.f ? DeltaTime : -DeltaTime) * 3.f;
	collTurnAdditive = FMath::Clamp(collTurnAdditive + dir, -2.f, 2.f);
}

void UBoatMovementComp::PushOtherBoat(ABoatPawn* otherBoat, FVector& ownerLocation, float force, float DeltaTime)
{
	auto otherSize = otherBoat->boatMovementComp->GetBoatSize();
	auto sizeDifference = FMath::Clamp(boatSize, 1, boatSize) / FMath::Clamp(otherSize, 1, otherSize);

	auto safeForward = owner->GetActorForwardVector().GetSafeNormal();
	auto otherSafeRight = otherBoat->GetActorRightVector().GetSafeNormal();

	auto forwardDot = FVector::DotProduct(safeForward, otherSafeRight);

	if (forwardDot > 0.8f || forwardDot < -0.8f)
	{
		auto otherLocation2D = FVector(otherBoat->GetActorLocation().X, otherBoat->GetActorLocation().Y, 0.f);
		auto ownerLocation2D = FVector(ownerLocation.X, ownerLocation.Y, 0.f);

		auto deltaLocation = FVector(otherLocation2D - ownerLocation2D).GetSafeNormal();

		otherSafeRight = (forwardDot > 0.f ? otherSafeRight : -otherSafeRight);

		auto sideCrossZ = FVector::CrossProduct(
			otherSafeRight,
			deltaLocation).GetSafeNormal().Z;

		auto turnDir = (sideCrossZ > 0.f ? DeltaTime : -DeltaTime) * moveVelocity * (15.f * (float)sizeDifference);
		otherBoat->boatMovementComp->IncreaseCollTurnAdditive(turnDir);
		otherBoat->ApplyCollisionDamage((50.f * FMath::Abs(moveVelocity)) * sizeDifference);
	}
	otherBoat->boatMovementComp->AddPushForce((force * (float)sizeDifference) * safeForward);
}

void UBoatMovementComp::UpdateBoatRotation(float DeltaTime)
{
	auto bShouldReverseTurning = moveVelocity < 0.f && throttle < 0.f;
	auto updatedYaw = GetUpdatedRotAxis(DeltaTime, turnSpeed, turnVelocity, bShouldReverseTurning ? -turnDirection : turnDirection, bUpdateTurnVel);

	FRotator newRotation = FRotator(0.f, updatedYaw, 0.f) * DeltaTime * FMath::Clamp(FMath::Abs(moveVelocity) * 2.f, 0.2f, 1.f);

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

void UBoatMovementComp::IncreaseCollTurnAdditive(float valueToAdd)
{
	collTurnAdditive = FMath::Clamp(collTurnAdditive + valueToAdd, -2.f, 2.f);
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