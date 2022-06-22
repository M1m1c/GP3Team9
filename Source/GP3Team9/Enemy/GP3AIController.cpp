#include "GP3AIController.h"


#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "GP3Team9/Boat/BoatPawn.h"
#include "Kismet/KismetMathLibrary.h"
#include "GP3Team9/Boat/BoatMovementComp.h"
#include "GP3Team9/Boat/GunDriverComp.h"

AGP3AIController::AGP3AIController()
{
	SwivelGunFireDelay = UKismetMathLibrary::RandomFloat() + 2; //Gun component delay is 2 secs (as of now)
	
}

void AGP3AIController::BeginPlay()
{
	Super::BeginPlay();
	Self = Cast<ABoatPawn>(GetPawn());
	StartPosition = GetPawn()->GetActorLocation();
	PlayerController = GetWorld()->GetFirstPlayerController();
	if (PlayerController)
	{
		Self->gunDriverComp->SetTarget(PlayerController);
	}
}

void AGP3AIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	TargetPlayer = PlayerController->GetPawn();
	
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
	
	if (BPlayerIsWithinSight())
	{
		CurrentState = EAIState::Chasing;
		LastKnownPlayerPosition = HitResult.Location;
		return;
	}

	if (FVector::Distance(GetPawn()->GetActorLocation(), StartPosition) > PerimeterRadius)
	{
		TurnAround();
		return;
	}
	
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
	//DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation(), FColor::Blue);

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
	
	if (BPlayerIsWithinSight())
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
	Logger("Player within shooting range");
	
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

	// Get angle towards player in -pi to pi. Zero is straight ahead.
	FVector PlayerLocationInAILocalSpace = UKismetMathLibrary::InverseTransformLocation(
		GetPawn()->GetTransform(), TargetPlayer->GetActorLocation());
	float AngleToPlayer = FMath::Atan2(PlayerLocationInAILocalSpace.Y , PlayerLocationInAILocalSpace.X);
	
	// Unless player is "directly" behind player, fire swivel gun.
	FFloatRange SwivelGunAngleRange = FFloatRange(-2.0f, 2.0f);
	if (SwivelGunAngleRange.Contains(AngleToPlayer))
	{
		if (SwivelGunTimeSinceLastFire < SwivelGunFireDelay)
		{
			SwivelGunTimeSinceLastFire += GetWorld()->DeltaTimeSeconds;
			return;
		}
		
		Self->gunDriverComp->FireSwivelGunAtLocation(GetRandomizedFiringTargetPosition());
		Logger("Firing Swivel gun");
		SwivelGunFireDelay = UKismetMathLibrary::RandomFloat() + 2;
		SwivelGunTimeSinceLastFire = 0;
	}
	
	// If angle to player is "sideways", shoot side gun. Otherwise, rotate to get there.
	float Range = SideGunAngleRange / 360 * UKismetMathLibrary::GetTAU();
	float SideGunMaxAngleRear = UKismetMathLibrary::GetPI() / 2 + Range / 2;
	float SideGunMaxAngleFront = UKismetMathLibrary::GetPI() / 2 - Range / 2;
	FFloatRange LeftGunAngleRange = FFloatRange(-SideGunMaxAngleRear, -SideGunMaxAngleFront);
	FFloatRange RightGunAngleRange = FFloatRange(SideGunMaxAngleFront, SideGunMaxAngleRear);
	
	if (RightGunAngleRange.Contains(AngleToPlayer))
	{
		Logger("Firing right gun");
		Self->gunDriverComp->FireRightGunsAtLocation(GetRandomizedFiringTargetPosition());
		return;
	}

	if (LeftGunAngleRange.Contains(AngleToPlayer))
	{
		Logger("Firing left gun");
		Self->gunDriverComp->FireLeftGunsAtLocation(GetRandomizedFiringTargetPosition());
		return;
	}
	
	if (AngleToPlayer > SideGunMaxAngleRear ||
		AngleToPlayer > -SideGunMaxAngleFront && AngleToPlayer < 0)
	{
		Self->boatMovementComp->ReadTurning(-0.1f);
		Logger("Rotating RIGHT");
		return;
	}
	
	if (AngleToPlayer < -SideGunMaxAngleRear ||
		AngleToPlayer < SideGunMaxAngleFront && AngleToPlayer > 0)
	{
		Self->boatMovementComp->ReadTurning(0.1f);
		Logger("Rotating LEFT");
	}
}

FVector AGP3AIController::GetRandomizedFiringTargetPosition()
{
	float Multiplier = TargetBoxSize;
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

void AGP3AIController::TurnAround()
{
	FVector Position = GetPawn()->GetActorLocation();
	float AngleToStartPos = FVector(StartPosition - Position).HeadingAngle();
	float Direction = -1;
	
	if (AngleToStartPos < 0)
	{
		Direction = 1;
	}

	Self->boatMovementComp->ReadTurning(0.1f * Direction);
}

bool AGP3AIController::BIsDirectionClear(const FVector Direction)
{
	FVector DirectionWithZeroZ = Direction + FVector(0,0,-Direction.Z);
	FVector Location = GetPawn()->GetActorLocation();
	FHitResult HitRes;
	FVector Start = Location + DirectionWithZeroZ * 500;
	FVector End = Location + DirectionWithZeroZ * 5000;
	float HullDepthOffset = -400;
	Start.Z = HullDepthOffset;
	End.Z = HullDepthOffset;
	FCollisionShape Shape = FCollisionShape::MakeBox(FVector(100, 200, 100));
	
	GetWorld()->SweepSingleByChannel(HitRes, Start, End, FQuat::Identity, ECC_WorldStatic, Shape);

	if (HitRes.Actor == nullptr)
	{
		// DrawDebugLine(GetWorld(), Start, End, FColor::Green, false, 0.1f, 0, 10);
		return true;
	}
	
	{
		// DrawDebugLine(GetWorld(), Start, HitRes.Location, FColor::Red, false, 0.1f, 0, 10);
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

bool AGP3AIController::BPlayerIsWithinSight()
{
	CheckLineOfSightToPlayer();
	float DistanceToPlayer = FVector::Distance(GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation());
	if (HitResult.GetActor() == TargetPlayer && DistanceToPlayer < MaxChasingDistance)
	{
		return HitResult.GetActor() == TargetPlayer;
	}
	return false;
}

void AGP3AIController::Logger(FString input)
{
	return;
	if (input == LastLog)
	{
		return;
	}
	LastLog = input;
	UE_LOG(LogTemp, Warning, TEXT("%s"), *input);
}



