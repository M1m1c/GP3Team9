
#include "BoatPawn.h"

#include "CameraDriverComp.h"
#include "CameraShakeComp.h"
#include "BoatMovementComp.h"
#include "GunDriverComp.h"
#include "FloatMasterComp.h"
#include "HealthComp.h"
#include "CrewComp.h"
#include "UpgradeManagerComp.h"
#include "ShipGun.h"
//#include "BoatUpgrade.h"
//#include "ArmorUpgrade.h"
#include "FloatingProp.h"
#include "IceBreakerComp.h"


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

	meshOffset = CreateDefaultSubobject<USceneComponent>(FName("MeshOffset"));
	meshOffset->SetupAttachment(meshHolder);

	graphicsHolder = CreateDefaultSubobject<USceneComponent>(FName("GraphicsHolder"));
	graphicsHolder->SetupAttachment(meshOffset);

	ArmorTier1Graphics = CreateDefaultSubobject<USceneComponent>(FName("ArmorTier1Graphics"));
	ArmorTier1Graphics->SetupAttachment(graphicsHolder);

	ArmorTier2Graphics = CreateDefaultSubobject<USceneComponent>(FName("ArmorTier2Graphics"));
	ArmorTier2Graphics->SetupAttachment(graphicsHolder);

	ArmorTier3Graphics = CreateDefaultSubobject<USceneComponent>(FName("ArmorTier3Graphics"));
	ArmorTier3Graphics->SetupAttachment(graphicsHolder);

	IceBreakerMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName("IceBreakerMesh"));
	IceBreakerMesh->SetupAttachment(graphicsHolder);

	cameraHolder = CreateDefaultSubobject<USceneComponent>(TEXT("cameraHolder"));
	cameraHolder->SetupAttachment(root);

	cameraArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraArm"));
	cameraArm->SetupAttachment(cameraHolder);
	cameraArm->TargetArmLength = 300.0f;

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	cameraComp->SetupAttachment(cameraArm, USpringArmComponent::SocketName);

	cameraDriver = CreateDefaultSubobject<UCameraDriverComp>(TEXT("CameraDriver"));

	cameraShaker = CreateDefaultSubobject<UCameraShakeComp>(TEXT("CameraShakeComp"));

	boatMovementComp = CreateDefaultSubobject<UBoatMovementComp>(TEXT("BoatMovementComp"));

	gunDriverComp = CreateDefaultSubobject<UGunDriverComp>(TEXT("GunDriverComp"));

	floatMasterComp = CreateDefaultSubobject<UFloatMasterComp>(TEXT("FloatMasterComp"));

	healthComp = CreateDefaultSubobject<UHealthComp>(TEXT("HealthComp"));

	crewComp = CreateDefaultSubobject<UCrewComp>(TEXT("CrewComp"));

	iceBreakerComp = CreateDefaultSubobject<UIceBreakerComp>(TEXT("IceBreakerComp"));

	//upgradeComp = CreateDefaultSubobject<UUpgradeManagerComp>(TEXT("UpgradeComp"));
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
	//(X=0.282589,Y=0.282589,Z=0.282589)
	meshOffset->SetUsingAbsoluteScale(true);
	meshOffset->SetWorldScale3D(FVector(0.282589f, 0.282589f, 0.282589f));
	auto startLocation = GetActorLocation();
	pushCollider->SetWorldLocation(startLocation);
	cameraDriver->Initalize(cameraHolder, cameraArm);
	cameraShaker->Initalize();
	boatMovementComp->Initalize();
	gunDriverComp->Initalize(cameraHolder);
	healthComp->Initalize(gunDriverComp);
	crewComp->Initalize();
	pushCollider->SetCollisionProfileName(FName("BoatPush"));


	ArmorTier1Graphics->bHiddenInGame = true;
	ArmorTier2Graphics->bHiddenInGame = true;
	ArmorTier3Graphics->bHiddenInGame = true;
	IceBreakerMesh->SetHiddenInGame(true);
	//IceBreakerMesh->bHiddenInGame = true;
	//IceBreakerMesh->SetCollisionProfileName(FName("NoCollision"));

	healthComp->OnBoatDeath.AddDynamic(this, &ABoatPawn::OnBoatDeath);
	healthComp->OnSectionDestroyed.AddDynamic(this, &ABoatPawn::OnSectionDestroyed);
	healthComp->OnSectionRepaired.AddDynamic(this, &ABoatPawn::OnSectionRepaired);
	//healthComp->OnSectionHit.AddDynamic(this, &ABoatPawn::OnSectionHit);
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
			if (boatMovementComp->bInitalized) {
				boatMovementComp->UpdateBoatMovement(DeltaTime);
				boatMovementComp->UpdateBoatRotation(DeltaTime);
			}
		}
		UpdateActorZPosition(DeltaTime);
		previousDeltaTime = DeltaTime;
	}
	if (GetActorLocation().Z < -9500.f) { DestroyBoat(); }
	collisionTimer = FMath::Clamp(collisionTimer - DeltaTime, 0.f, maxCollisionTimer);
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
	OnBoatDeathEvent();
	bIsBoatDead = true;
	floatMasterComp->DisableFloating();

	if (!itemToDropOnDeath) { return; }
	auto location = GetActorLocation() + FVector(0.f, 0.f, 500.f);
	auto rotation = GetActorRotation();
	GetWorld()->SpawnActor<AFloatingProp>(itemToDropOnDeath, location, rotation);
}

void ABoatPawn::OnSendCrew(int startSection, int endSection)
{
	OnSendCrewToSection(startSection, endSection);
}

void ABoatPawn::OnSectionDestroyed(UHealthSection* sectionAffected)
{
	OnSectionDestroyedEvent(sectionAffected);
}

void ABoatPawn::OnSectionRepaired(UHealthSection* sectionAffected)
{
	OnSectionRepairedEvent(sectionAffected);
}

void ABoatPawn::OnSectionHit()
{
	OnSectionHitEvent();
}

void ABoatPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	InputComponent->BindAxis("CameraHorizontal", cameraDriver, &UCameraDriverComp::ReadCameraHorizontal);
	InputComponent->BindAxis("CameraVertical", cameraDriver, &UCameraDriverComp::ReadCameraVertical);
	//InputComponent->BindAxis("CameraZoom", cameraDriver, &UCameraDriverComp::ReadCameraZoom);
	InputComponent->BindAction("FreeCameraLook", IE_Pressed, cameraDriver, &UCameraDriverComp::ToggleCameraFreeLook);

	//InputComponent->BindAction("AllowGearChange", IE_Pressed, boatMovementComp, &UBoatMovementComp::AllowGearChange);
	//InputComponent->BindAction("DisallowGearChange", IE_Released, boatMovementComp, &UBoatMovementComp::DisallowGearChange);

	InputComponent->BindAxis("GearChange", boatMovementComp, &UBoatMovementComp::ReadGearChange);
	InputComponent->BindAxis("TurnDirection", boatMovementComp, &UBoatMovementComp::ReadTurning);

	InputComponent->BindAction("FireLeft", IE_Pressed, gunDriverComp, &UGunDriverComp::FireLeftGuns);
	//InputComponent->BindAction("FireLeft", IE_Pressed, cameraDriver, &UCameraDriverComp::AimLeft);
	InputComponent->BindAction("FireLeft", IE_Released, gunDriverComp, &UGunDriverComp::StopFireLeftGuns);
	InputComponent->BindAction("FireLeft", IE_Pressed, gunDriverComp, &UGunDriverComp::AimLeftGuns);
	InputComponent->BindAction("FireLeft", IE_Released, cameraDriver, &UCameraDriverComp::StopAimLeft);

	InputComponent->BindAction("FireRight", IE_Pressed, gunDriverComp, &UGunDriverComp::FireRightGuns);
	//InputComponent->BindAction("FireRight", IE_Pressed, cameraDriver, &UCameraDriverComp::AimRight);
	InputComponent->BindAction("FireRight", IE_Released, gunDriverComp, &UGunDriverComp::StopFireRightGuns);
	InputComponent->BindAction("FireRight", IE_Pressed, gunDriverComp, &UGunDriverComp::AimRightGuns);
	InputComponent->BindAction("FireRight", IE_Released, cameraDriver, &UCameraDriverComp::StopAimRight);

	InputComponent->BindAction("FireSwivel", IE_Pressed, gunDriverComp, &UGunDriverComp::FireSwivelGun);
	InputComponent->BindAction("FireSwivel", IE_Released, gunDriverComp, &UGunDriverComp::StopFireSwivelGun);

	InputComponent->BindAxis("MoveCrewX", crewComp, &UCrewComp::MoveCrewMemberX);
	InputComponent->BindAxis("MoveCrewY", crewComp, &UCrewComp::MoveCrewMemberY);


	//InputComponent->BindAction("UpgradeTest", IE_Pressed, this, &ABoatPawn::UpgradeTest);
	InputComponent->BindAction("UpgradeTest", IE_Pressed, this, &ABoatPawn::LevelUpBoat);

}


void ABoatPawn::ApplyCollisionDamage(float damage)
{
	if (!FMath::IsNearlyZero(collisionTimer)) { return; }
	collisionTimer = maxCollisionTimer;
	healthComp->ApplyDamageToAllSections(damage);
}

void ABoatPawn::ApplyCollisionShake(float trauma)
{
	auto currentTrauma = cameraShaker->GetTrauma();
	auto traumaToAdd = trauma - currentTrauma;
	if (traumaToAdd <= 0.f) { return; }
	cameraShaker->AddShakeTrauma(traumaToAdd);
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

UStaticMeshComponent* ABoatPawn::GetFloatBody()
{
	return boatBody;
}

void ABoatPawn::SwapBoat(TSubclassOf<ABoatPawn> boatToSwapTo)
{
	auto myController = GetController();
	auto location = GetActorLocation() + FVector(0.f, 0.f, 500.f);
	auto rotation = GetActorRotation();

	auto newBoat = GetWorld()->SpawnActor<ABoatPawn>(boatToSwapTo, location, rotation);
	newBoat->crewComp->SetCrewCount(crewComp->GetCrewCount());
	newBoat->iceBreakerComp->IncreaseSegments(iceBreakerComp->GetSegementCount());
	newBoat->boatMovementComp->SetGear(boatMovementComp->GetGear());
	myController->Possess(newBoat);
	OnBoatSwapEvent(newBoat);
}

void ABoatPawn::DestroyBoat()
{
	gunDriverComp->DestroyGuns();
	Destroy();
}

TMap<EGunSlotPosition, class AShipGun*> ABoatPawn::GetShipGuns()
{
	return gunDriverComp->GetEachDirectionShipGuns();
}

void ABoatPawn::LevelUpBoat()
{
	if (!NextLevelBoat) { return; }
	SwapBoat(NextLevelBoat);
}

//void ABoatPawn::UpgradeTest()
//{
//	if (!boatUpgrade) { return; }
//	auto instance = NewObject<UArmorUpgrade>(boatUpgrade);
//	upgradeComp->ApplyUpgrade(Cast<IBoatUpgrade>(instance));
//}
