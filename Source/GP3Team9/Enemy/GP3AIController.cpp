#include "GP3AIController.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "GP3Team9/Boat/BoatPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GP3Team9/Boat/BoatMovementComp.h"
#include "GP3Team9/Boat/GunDriverComp.h"
// #include "GameFramework/Actor.h"

AGP3AIController::AGP3AIController()
{
}

void AGP3AIController::BeginPlay()
{
	Super::BeginPlay();
	Self = Cast<ABoatPawn>(GetPawn());
}

void AGP3AIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (TargetPlayer == nullptr)
	{
		return;
	}
	
	CheckLineOfSightToPlayer();
	
	switch (CurrentState)
	{
	case EAIState::Cruising :
		Cruise();
		Self->boatMovementComp->SetGear(2);
		Self->boatMovementComp->turnSpeed = 20;
		break;
	case EAIState::Chasing :
		ChasePlayer();
		Self->boatMovementComp->SetGear(3);
		Self->boatMovementComp->turnSpeed = 20;
		break;
	case EAIState::GoingToLastKnownPosition :
		GoToLastKnownPosition();
		Self->boatMovementComp->SetGear(3);
		Self->boatMovementComp->turnSpeed = 20;
		break;
	case EAIState::Attacking :
		AttackPlayer();
		Self->boatMovementComp->SetGear(1);
		Self->boatMovementComp->turnSpeed = 40;
		break;
	}
}

void AGP3AIController::Cruise()
{
	Logger("Just cruisin'");
	
	if (BHasLineOfSight())
	{
		CurrentState = EAIState::Chasing;
		LastKnownPlayerPosition = HitResult.Location;
		return;
	}
	
	float DistanceToCheck = 5000.0f;
	FVector ForwardVector = GetPawn()->GetActorForwardVector();
	float SteerInput = 0;
	
	bool ClearAhead = BIsDirectionClear(ForwardVector);
	if (!ClearAhead)
	{
		for (int Angle = 0; Angle < 160; Angle+=5)
		{
			FVector DirectionToCheck;
			float AngleInRad;

			AngleInRad = FMath::DegreesToRadians(Angle);
			DirectionToCheck.X = ForwardVector.X * FMath::Cos(AngleInRad) - ForwardVector.Y * FMath::Sin(AngleInRad);
			DirectionToCheck.Y = ForwardVector.X * FMath::Sin(AngleInRad) + ForwardVector.Y * FMath::Cos(AngleInRad);

			if (BIsDirectionClear(DirectionToCheck))
			{
				SteerInput = Angle;
				break;
			}

			AngleInRad = FMath::DegreesToRadians(-Angle);
			DirectionToCheck.X = ForwardVector.X * FMath::Cos(AngleInRad) - ForwardVector.Y * FMath::Sin(AngleInRad);
			DirectionToCheck.Y = ForwardVector.X * FMath::Sin(AngleInRad) + ForwardVector.Y * FMath::Cos(AngleInRad);

			if (BIsDirectionClear(DirectionToCheck))
			{
				SteerInput = -Angle;
				break;
			}
		}
	}

	float SteerModifier = 0.0001f;
	Self->boatMovementComp->ReadTurning(FMath::Clamp(SteerInput, -1.0f, 1.0f) * SteerModifier);
}


void AGP3AIController::ChasePlayer()
{
	Logger("Player spotted, chasing");
	DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation(), FColor::Blue);

	LastKnownPlayerPosition = TargetPlayer->GetActorLocation();
	
	if (GetPawn()->GetDistanceTo(TargetPlayer) < MaxAttackDistance)
	{
		CurrentState = EAIState::Attacking;
		return;
	}

	SteerTowardsLocation(TargetPlayer->GetActorLocation());
	
	if (HitResult.Actor != TargetPlayer)
	{
		CurrentState = EAIState::GoingToLastKnownPosition;
	}
}

void AGP3AIController::GoToLastKnownPosition()
{
	Logger("Checking last known player position");
	
	if (BHasLineOfSight())
	{
		CurrentState = EAIState::Chasing;
		LastKnownPlayerPosition = HitResult.Location;
		return;
	}
	
	SteerTowardsLocation(LastKnownPlayerPosition);

	auto X = FFloatRange(LastKnownPlayerPosition.X - 300, LastKnownPlayerPosition.X + 300);
	auto Y = FFloatRange(LastKnownPlayerPosition.Y - 300, LastKnownPlayerPosition.Y + 300);

	
	if (X.Contains(Self->GetActorLocation().X) && Y.Contains(Self->GetActorLocation().Y))
	{
		CurrentState = EAIState::Cruising;
	}
}

void AGP3AIController::AttackPlayer()
{
	Logger("Player withing shooting range");
	
	LastKnownPlayerPosition = TargetPlayer->GetActorLocation();

	if (HitResult.Actor != TargetPlayer)
	{
		CurrentState = EAIState::GoingToLastKnownPosition;
		return;
	}
	
	if (GetPawn()->GetDistanceTo(TargetPlayer) > MaxAttackDistance)
	{
		CurrentState = EAIState::Chasing;
		return;
	}
	
	Self->gunDriverComp->FireSwivelGunAtLocation(GetRandomizedFiringTargetPosition());
	Logger("Firing Swivel gun");

	// If angle to player is "sideways", shoot side gun. Otherwise, turn.
	FVector PlayerLocationInAILocalSpace = UKismetMathLibrary::InverseTransformLocation(
		GetPawn()->GetTransform(), TargetPlayer->GetActorLocation());
	float Angle = FMath::Atan2(PlayerLocationInAILocalSpace.Y , PlayerLocationInAILocalSpace.X);

	float GunAngleHigh = 1.8f;
	float GunAngleLow = 1.2f;
	
	if (Angle > GunAngleLow && Angle < GunAngleHigh)
	{
		Logger("Firing right gun");
		Self->gunDriverComp->FireRightGunsAtLocation(GetRandomizedFiringTargetPosition());
		return;
	}

	if (Angle < -GunAngleHigh && Angle > -GunAngleLow)
	{
		Logger("Firing left gun");
		Self->gunDriverComp->FireLeftGunsAtLocation(GetRandomizedFiringTargetPosition());
		return;
	}
	
	if (Angle > GunAngleHigh ||
		Angle > -GunAngleLow && Angle < 0)
	{
		Self->boatMovementComp->ReadTurning(0.1f);
		Logger("Rotating LEFT");
		return;
	}
	
	if (Angle < -GunAngleHigh ||
		Angle < GunAngleLow && Angle > 0)
	{
		Self->boatMovementComp->ReadTurning(-0.1f);
		Logger("Rotating RIGHT");
	}
}

FVector AGP3AIController::GetRandomizedFiringTargetPosition()
{
	float Multiplier = 500;
	float Offset = UKismetMathLibrary::RandomFloat() * Multiplier - Multiplier / 2;
	FVector OffsetVector = FVector(Offset);
	
	return TargetPlayer->GetActorLocation() + OffsetVector;
}

void AGP3AIController::SteerTowardsLocation(FVector TargetLocation)
{
	FVector LocationInAILocalSpace = UKismetMathLibrary::InverseTransformLocation(
	GetPawn()->GetTransform(), TargetLocation);
	float Angle = FMath::Atan2(LocationInAILocalSpace.Y , LocationInAILocalSpace.X);

	// Turn accordingly
	if (Angle > 0.2f)
	{
		Self->boatMovementComp->ReadTurning(0.1f);
	}
	if (Angle < 0.2f)
	{
		Self->boatMovementComp->ReadTurning(-0.1f);
	}
}

bool AGP3AIController::BIsDirectionClear(const FVector Direction)
{
	FVector DirectionWithZeroZ = Direction + FVector(0,0,-Direction.Z);
	FVector Location = GetPawn()->GetActorLocation();
	FHitResult HitRes;
	FVector Start = Location + DirectionWithZeroZ * 500;
	FVector End = Location + DirectionWithZeroZ * 5000;
	FCollisionShape Shape = FCollisionShape::MakeBox(FVector(100, 200, 100));
	
	GetWorld()->SweepSingleByChannel(HitRes, Start, End, FQuat::Identity, ECC_WorldStatic, Shape);

	if (HitRes.Actor == nullptr)
	{
		DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.1f, 0, 10);
		return true;
	}
	
	{
		DrawDebugLine(GetWorld(), Start, HitRes.Location, FColor::Red, false, 0.1f, 0, 10);
		return false;
	}
}

void AGP3AIController::CheckLineOfSightToPlayer()
{
	// FHitResult HitResult;
	FVector Direction = FVector((TargetPlayer->GetActorLocation() - GetPawn()->GetActorLocation()).GetSafeNormal());
	FVector TraceStart = GetPawn()->GetActorLocation() + Direction * 500;
	FVector TraceEnd = TargetPlayer->GetActorLocation();
	
	GetWorld()->SweepSingleByChannel(HitResult, TraceStart, TraceEnd, FQuat::Identity,
	ECC_Pawn, FCollisionShape::MakeSphere(1.0f));
	
}

bool AGP3AIController::BHasLineOfSight()
{
	CheckLineOfSightToPlayer();
	return HitResult.GetActor() == TargetPlayer;
}

void AGP3AIController::Logger(FString input)
{
	if (input == LastLog)
	{
		return;
	}
	LastLog = input;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *input);
}



