// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveController.h"
#include "WaveHeightmap.h"
#include "Components/StaticMeshComponent.h"

// Sets default values
AWaveController::AWaveController()
{
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWaveController::BeginPlay()
{
	Super::BeginPlay();

	WaveHeightMapAsset->surfaceLevelHeight = GetActorLocation().Z;
	
	const auto mesh = Cast<UStaticMeshComponent>(GetComponentByClass(UStaticMeshComponent::StaticClass()));
	WavesMaterialInstance = UMaterialInstanceDynamic::Create(mesh->GetMaterial(0), nullptr);

	bool failed = false;

	if (WavesMaterialInstance == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Wave Material not cast correctly !!"));
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Wave Material not cast correctly !!"));
		failed = true;
	}

	if (WaveHeightMapAsset == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Wave Data Asset not set in editor!!"));
		GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Red, TEXT("Wave Data Asset not set in editor!!"));
		failed = true;
	}

	if (failed)
	{
		return;
	}

	for (int i = 0; i < WaveHeightMapAsset->waves.Num(); i++)
	{
		float waveLeng = WaveHeightMapAsset->waves[i].waveLength;
		float waveAmp = WaveHeightMapAsset->waves[i].waveAmplitude;
		float waveSpd = WaveHeightMapAsset->waves[i].waveSpeed;
		float waveSteep = WaveHeightMapAsset->waves[i].waveSteepness;
		FVector waveDir = WaveHeightMapAsset->waves[i].waveDirection;

		FString waveIndex = FString::FromInt(i + 1);

		FString wLenFString = "Wave" + waveIndex + "Length";
		FString wAmpFString = "Wave" + waveIndex + "Amplitude";
		FString wSpdFString = "Wave" + waveIndex + "Speed";
		FString wSteepFString = "Wave" + waveIndex + "Steepness";
		FString wDirFString = "Wave" + waveIndex + "Direction";

		FName wLenName = FName(wLenFString);
		FName wAmpName = FName(wAmpFString);
		FName wSpdName = FName(wSpdFString);
		FName wSteepName = FName(wSteepFString);
		FName wDirName = FName(wDirFString);

		WavesMaterialInstance->SetScalarParameterValue(wLenName, waveLeng);
		WavesMaterialInstance->SetScalarParameterValue(wAmpName, waveAmp);
		WavesMaterialInstance->SetScalarParameterValue(wSpdName, waveSpd);
		WavesMaterialInstance->SetScalarParameterValue(wSteepName, waveSteep);
		WavesMaterialInstance->SetVectorParameterValue(wDirName, waveDir);
	}

	mesh->SetMaterial(0, WavesMaterialInstance);
}

// Called every frame
void AWaveController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
