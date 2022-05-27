

#include "GP3Enemy.h"
#include "GameFramework/Pawn.h"
#include "DrawDebugHelpers.h"


AGP3Enemy::AGP3Enemy()
{
	PrimaryActorTick.bCanEverTick = true;
}


void AGP3Enemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGP3Enemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckLineOfSightToPlayer();
	
	switch (CurrentState)
	{
	case EAIState::Cruising : Cruising(); break;
	case EAIState::Chasing : ChasePlayer(); break;
	case EAIState::Attacking : AttackPlayer(); break;
	}

}

void AGP3Enemy::Cruising()
{

	DrawDebugLine(GetWorld(), GetActorLocation(), TargetPlayer->GetActorLocation(), FColor::Red);
	
	if (HitResult.Actor == TargetPlayer)
	{
		CurrentState = EAIState::Chasing;
		UE_LOG(LogTemp, Warning, TEXT("Player spotted"));
	}
}

void AGP3Enemy::ChasePlayer()
{
	if (GetDistanceTo(TargetPlayer) < MaxAttackDistance)
	{
		CurrentState = EAIState::Attacking;
		UE_LOG(LogTemp, Warning, TEXT("Attacking Player"));
		return;
	}
	
	//Move towards player


	DrawDebugLine(GetWorld(), GetActorLocation(), TargetPlayer->GetActorLocation(), FColor::Blue);

	if (HitResult.Actor != TargetPlayer)
	{
		CurrentState = EAIState::Cruising;
		UE_LOG(LogTemp, Warning, TEXT("Player lost"));
	}
}

void AGP3Enemy::AttackPlayer()
{
	// Shoot at player

	DrawDebugLine(GetWorld(), GetActorLocation(), TargetPlayer->GetActorLocation(), FColor::Green);

	if (HitResult.Actor != TargetPlayer)
	{
		CurrentState = EAIState::Cruising;
		UE_LOG(LogTemp, Warning, TEXT("Player lost"));
		return;
	}

	if (GetDistanceTo(TargetPlayer) > MaxAttackDistance)
	{
		CurrentState = EAIState::Chasing;
		UE_LOG(LogTemp, Warning, TEXT("Chasing player"));
	}
}

void AGP3Enemy::CheckLineOfSightToPlayer()
{
	FVector TraceStartOffset = TargetPlayer->GetActorLocation() - GetActorLocation();
	TraceStartOffset.Normalize();
	FVector TraceStart = GetActorLocation() + TraceStartOffset * 100;
	FVector TraceEnd = TargetPlayer->GetActorLocation();
	
	GetWorld()->SweepSingleByChannel(HitResult, TraceStart, TraceEnd, FQuat::Identity,
	ECC_GameTraceChannel2, FCollisionShape::MakeSphere(100.0f));
}

