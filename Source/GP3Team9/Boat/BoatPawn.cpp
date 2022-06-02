
#include "BoatPawn.h"

#include "CameraDriverComp.h"
#include "BoatMovementComp.h"
#include "GunDriverComp.h"
#include "FloatMasterComp.h"
#include "HealthComp.h"
#include "CrewComp.h"

#include "Components/StaticMeshComponent.h"
#include <Runtime/Engine/Classes/GameFramework/SpringArmComponent.h>
#include <Runtime/Engine/Classes/Camera/CameraComponent.h>
#include "Components/CapsuleComponent.h"

ABoatPawn::ABoatPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	root = CreateDefaultSubobject<USceneComponent>(FName("Root"));
	SetRootComponent(root);

	pushCollider = CreateDefaultSubobject<UCapsuleComponent>(FName("PushCollider"));
	pushCollider->SetupAttachment(root);
	pushCollider->SetUsingAbsoluteLocation(true);

	boatBody = CreateDefaultSubobject<UStaticMeshComponent>(FName("BoatBody"));
	boatBody->SetupAttachment(root);

	meshHolder = CreateDefaultSubobject<USceneComponent>(FName("MeshHolder"));
	meshHolder->SetupAttachment(boatBody);

	cameraHolder = CreateDefaultSubobject<USceneComponent>(TEXT("cameraHolder"));
	cameraHolder->SetupAttachment(root);

	cameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	cameraArm->SetupAttachment(cameraHolder);
	cameraArm->TargetArmLength = 300.0f;


	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	cameraComp->SetupAttachment(cameraArm, USpringArmComponent::SocketName);

	cameraDriver = CreateDefaultSubobject<UCameraDriverComp>(TEXT("CameraDriver"));

	boatMovementComp = CreateDefaultSubobject<UBoatMovementComp>(TEXT("BoatMovementComp"));

	gunDriverComp = CreateDefaultSubobject<UGunDriverComp>(TEXT("GunDriverComp"));

	floatMasterComp = CreateDefaultSubobject<UFloatMasterComp>(TEXT("FloatMasterComp"));

	healthComp = CreateDefaultSubobject<UHealthComp>(TEXT("HealthComp"));

	crewComp = CreateDefaultSubobject<UCrewComp>(TEXT("CrewComp"));
}

void ABoatPawn::BeginPlay()
{
	Super::BeginPlay();
	boatBody->SetSimulatePhysics(true);
	boatBody->SetEnableGravity(false);
	//boatBody->SetLinearDamping(2.f);
	//boatBody->SetAngularDamping(5.f);
	defaultAngularDamping = boatBody->GetAngularDamping();
	defaultLinearDamping = boatBody->GetLinearDamping();
	auto startLocation = GetActorLocation();
	pushCollider->SetWorldLocation(startLocation);
	cameraDriver->Initalize(cameraHolder, cameraArm);
	boatMovementComp->Initalize();
	gunDriverComp->Initalize(cameraHolder);
	healthComp->Initalize(gunDriverComp);
	pushCollider->SetCollisionProfileName(FName("BoatPush"));

	healthComp->OnBoatDeath.AddDynamic(this, &ABoatPawn::OnBoatDeath);
	crewComp->OnSendCrew.AddDynamic(this, &ABoatPawn::OnSendCrew);
}

void ABoatPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (boatBody)
	{
		if (FMath::IsNearlyZero(previousDeltaTime)) { previousDeltaTime = DeltaTime; }
		auto fps = previousDeltaTime / 1.f;
		boatBody->SetLinearDamping(FMath::Clamp(FMath::Lerp(defaultLinearDamping, 50.f, fps), defaultLinearDamping, 50.f));
		boatBody->SetAngularDamping(FMath::Clamp(FMath::Lerp(defaultAngularDamping, 50.f, fps), defaultLinearDamping, 50.f));

		if (boatMovementComp) {
			if (boatMovementComp->Initalized) {
				boatMovementComp->UpdateBoatMovement(DeltaTime);
				boatMovementComp->UpdateBoatRotation(DeltaTime);
			}
		}
		UpdateActorZPosition(DeltaTime);
		previousDeltaTime = DeltaTime;
	}
	if (GetActorLocation().Z < -9500.f) { Destroy(); }
}

bool ABoatPawn::GetIsInCameraMode()
{
	bool retval = false;
	if (cameraDriver)
	{
		retval = cameraDriver->GetbFreeCameraLook();
	}
	return retval;
}

void ABoatPawn::UpdateActorZPosition(float DeltaTime)
{
	auto currentLoc = GetActorLocation();
	auto bodyLoc = boatBody->GetComponentLocation();
	auto target = FVector(currentLoc.X, currentLoc.Y, FMath::Lerp(currentLoc.Z, bodyLoc.Z, DeltaTime));
	SetActorLocation(target);

	auto currentPushLoc = pushCollider->GetComponentLocation();
	pushCollider->SetWorldLocation(FVector(currentPushLoc.X, currentPushLoc.Y, bodyLoc.Z));
}

void ABoatPawn::OnBoatDeath()
{
	if (bIsBoatDead) { return; }
	bIsBoatDead = true;
	//boatBody->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
	floatMasterComp->DisableFloating();
	//boatBody->SetAllPhysicsLinearVelocity(FVector::ZeroVector);
}

void ABoatPawn::OnSendCrew(int startSection, int endSection)
{
	OnSendCrewToSection(startSection, endSection);
}

void ABoatPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("CameraHorizontal", cameraDriver, &UCameraDriverComp::ReadCameraHorizontal);
	InputComponent->BindAxis("CameraVertical", cameraDriver, &UCameraDriverComp::ReadCameraVertical);
	//InputComponent->BindAxis("CameraZoom", cameraDriver, &UCameraDriverComp::ReadCameraZoom);
	InputComponent->BindAction("FreeCameraLook", IE_Pressed, cameraDriver, &UCameraDriverComp::ToggleCameraFreeLook);

	InputComponent->BindAction("AllowGearChange", IE_Pressed, boatMovementComp, &UBoatMovementComp::AllowGearChange);
	InputComponent->BindAction("DisallowGearChange", IE_Released, boatMovementComp, &UBoatMovementComp::DisallowGearChange);

	InputComponent->BindAxis("GearChange", boatMovementComp, &UBoatMovementComp::ReadGearChange);
	InputComponent->BindAxis("TurnDirection", boatMovementComp, &UBoatMovementComp::ReadTurning);

	InputComponent->BindAction("FireLeft", IE_Released, gunDriverComp, &UGunDriverComp::FireLeftGuns);
	InputComponent->BindAction("FireLeft", IE_Pressed, cameraDriver, &UCameraDriverComp::AimLeft);
	InputComponent->BindAction("FireLeft", IE_Pressed, gunDriverComp, &UGunDriverComp::AimLeftGuns);
	InputComponent->BindAction("FireLeft", IE_Released, cameraDriver, &UCameraDriverComp::StopAimLeft);

	InputComponent->BindAction("FireRight", IE_Released, gunDriverComp, &UGunDriverComp::FireRightGuns);
	InputComponent->BindAction("FireRight", IE_Pressed, cameraDriver, &UCameraDriverComp::AimRight);
	InputComponent->BindAction("FireRight", IE_Pressed, gunDriverComp, &UGunDriverComp::AimRightGuns);
	InputComponent->BindAction("FireRight", IE_Released, cameraDriver, &UCameraDriverComp::StopAimRight);

	InputComponent->BindAction("FireSwivel", IE_Pressed, gunDriverComp, &UGunDriverComp::FireSwivelGun);

	InputComponent->BindAxis("MoveCrewX", crewComp, &UCrewComp::MoveCrewMemberX);
	InputComponent->BindAxis("MoveCrewY", crewComp, &UCrewComp::MoveCrewMemberY);

}


float ABoatPawn::GetLinearVelocityChange(float deltaTime, float accelSpeed)
{
	auto velocityChange = deltaTime * accelSpeed;
	return velocityChange;
}

float ABoatPawn::GetProportionalVelocityChange(float deltaTime, float currentVelocity, float accelSpeed, float decelSpeed, bool changeCondition)
{
	auto proportionalDec = (deltaTime + (deltaTime * (decelSpeed * currentVelocity)));
	proportionalDec = decelSpeed < 0.f ? -proportionalDec : proportionalDec;
	auto velocityChange = (changeCondition ? deltaTime * accelSpeed : proportionalDec);
	return velocityChange;
}

float ABoatPawn::GetPropVelocityChangeConstantDec(float deltaTime, float currentVelocity, float accelSpeed, float decelSpeed, bool changeCondition)
{
	auto proportionalDec = -(deltaTime + (deltaTime * (decelSpeed * currentVelocity)));
	auto deceleration = currentVelocity > .0f ? proportionalDec : -deltaTime;
	auto velocityChange = (changeCondition ? deltaTime * accelSpeed : deceleration);
	return velocityChange;
}