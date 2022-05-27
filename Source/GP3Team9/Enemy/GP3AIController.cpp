#include "GP3AIController.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/Pawn.h"
#include "GP3Team9/Boat/BoatPawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GP3Team9/Boat/BoatMovementComp.h"
// #include "GameFramework/Actor.h"

AGP3AIController::AGP3AIController()
{
}

void AGP3AIController::BeginPlay()
{
	Super::BeginPlay();
	Self = Cast<ABoatPawn>(GetPawn());
	TargetPlayer = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn();
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
	case EAIState::Cruising : Cruise(); break;
	case EAIState::Chasing : ChasePlayer(); break;
	case EAIState::Attacking : AttackPlayer(); break;
	}
}

void AGP3AIController::Cruise()
{
	// UE_LOG(LogTemp, Warning, TEXT("Player not in sight"));
	
	DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation(), FColor::Red);
	
	Self->boatMovementComp->ReadGearChange(1);
	Self->boatMovementComp->ReadGearChange(1);

	float DistanceToCheck = 1000.0f;
	bool ClearAhead = BIsPathClear(GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * DistanceToCheck);
	float SteerInput;
	
	if (!ClearAhead)
	{
		for (int Angle = 0; Angle < 180; Angle+=5)
		{
			UE_LOG(LogTemp, Warning, TEXT("%d"), Angle)
			// FVector NormalizedOffset = FVector(FMath::Cos(Angle), FMath::Sin(Angle), 0);
			FVector DirectionToCheck = GetEndPointToCheck(Angle, DistanceToCheck);
			
			// SteerInput = FMath::Clamp((float)Angle, 0.0f, 1.0f);
			UE_LOG(LogTemp, Warning, TEXT("%f"), SteerInput)

			if (BIsPathClear(DirectionToCheck))
			{
				SteerInput = -Angle;
				break;
			}

			DirectionToCheck = GetEndPointToCheck(-Angle, DistanceToCheck);

			if (BIsPathClear(DirectionToCheck))
			{
				SteerInput = -Angle;
				break;
			}
		}
	}
	else
	{
		SteerInput = 0;
	}
	
	Self->boatMovementComp->ReadTurning(SteerInput);
	
	if (BHasLineOfSight())
	{
		CurrentState = EAIState::Chasing;
		LastKnownPlayerLocation = HitResult.Location;
		UE_LOG(LogTemp, Warning, TEXT("Player spotted"));
	}
}

FVector AGP3AIController::GetEndPointToCheck(float Angle, float Distance)
{
	float AngleInRad = FMath::DegreesToRadians(Angle);
	FVector NormalizedForward = GetPawn()->GetActorForwardVector().GetSafeNormal();
				
	float x = NormalizedForward.X * FMath::Cos(AngleInRad) -
		NormalizedForward.Y * FMath::Sin(AngleInRad);
	float y = NormalizedForward.X * FMath::Sin(AngleInRad) -
		NormalizedForward.Y * FMath::Cos(AngleInRad);
	return FVector(x, y, 0) * Distance;
}

bool AGP3AIController::BIsPathClear(const FVector Direction)
{
	FHitResult HitRes;
	GetWorld()->SweepSingleByChannel(
		HitRes,
		GetPawn()->GetActorLocation() + GetPawn()->GetActorForwardVector() * 2000,
		Direction,
		FQuat::Identity,
		ECC_WorldStatic, FCollisionShape::MakeSphere(500.0f)
	);

	// UE_LOG(LogTemp, Warning, TEXT("%f"), HitRes.Distance)
	DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), Direction, FColor::Red);

	if (HitRes.Actor == nullptr)
	{
		return true;
	}
	else
	{
		DrawDebugBox(GetWorld(), HitRes.Location, FVector(100,100,100), FColor::Red,false,
		0.3f, 0, 10);
		UE_LOG(LogTemp, Warning, TEXT("%s"), *HitRes.Actor->GetName());
		return false;
	}
}

void AGP3AIController::ChasePlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("Chasing player"));
	DrawDebugLine(GetWorld(), GetPawn()->GetActorLocation(), TargetPlayer->GetActorLocation(), FColor::Blue);

	LastKnownPlayerLocation = TargetPlayer->GetActorLocation();
	
	if (GetPawn()->GetDistanceTo(TargetPlayer) < MaxAttackDistance)
	{
		CurrentState = EAIState::Attacking;
		UE_LOG(LogTemp, Warning, TEXT("Player within shooting range"));
		return;
	}
	
	// Start Moving forward. (Using pawn move component)
	// Check direction to player
	FRotator RotatorTowardsPlayer = UKismetMathLibrary::MakeRotFromX(TargetPlayer->GetActorLocation());
	// turn towards player (using pawn Move component)
	
	if (HitResult.Actor != TargetPlayer)
	{
		CurrentState = EAIState::Cruising;
		UE_LOG(LogTemp, Warning, TEXT("Player lost"));
	}
}

void AGP3AIController::AttackPlayer()
{
	UE_LOG(LogTemp, Warning, TEXT("Attacking Player"));
	DrawDebugLine(GetWorld(), GetOwner()->GetActorLocation(), TargetPlayer->GetActorLocation(), FColor::Green);

	LastKnownPlayerLocation = TargetPlayer->GetActorLocation();
	
	// Shoot at player (Weapon component?)
	
	// if distance is > minDistance: Turn halfway towards player and go forward.
	
	
	if (HitResult.Actor != TargetPlayer)
	{
		CurrentState = EAIState::Cruising;
		UE_LOG(LogTemp, Warning, TEXT("Player lost"));
		return;
	}

	if (GetOwner()->GetDistanceTo(TargetPlayer) > MaxAttackDistance)
	{
		CurrentState = EAIState::Chasing;
		UE_LOG(LogTemp, Warning, TEXT("Player too far away"));
	}
}

void AGP3AIController::CheckLineOfSightToPlayer()
{
	FVector TraceStartOffset = TargetPlayer->GetActorLocation() - GetPawn()->GetActorLocation();
	TraceStartOffset.Normalize();
	FVector TraceStart = GetPawn()->GetActorLocation() + TraceStartOffset * 100;
	FVector TraceEnd = TargetPlayer->GetActorLocation();
	
	GetWorld()->SweepSingleByChannel(HitResult, TraceStart, TraceEnd, FQuat::Identity,
	ECC_GameTraceChannel2, FCollisionShape::MakeSphere(100.0f));
}

bool AGP3AIController::BHasLineOfSight()
{
	CheckLineOfSightToPlayer();
	return HitResult.GetActor() == TargetPlayer;
}


