// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "WaveHeightmap.generated.h"

USTRUCT(BlueprintType)
struct FWaveValues
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	// The horizontal stretch of the wave
	float waveLength;
	UPROPERTY(EditAnywhere)
	// The vertical stretch of the wave
	float waveAmplitude;
	UPROPERTY(EditAnywhere)
	// How quickly the wave travels along it's direction vector
	float waveSpeed;
	UPROPERTY(EditAnywhere)
	// how sharp the wavecrests look like
	float waveSteepness;
	UPROPERTY(EditAnywhere)
	// the direction which the wave will move towards -- (!) PLEASE KEEP Z = 0 (!)
	FVector waveDirection;
};

UCLASS()
class GP3TEAM9_API UWaveHeightmap : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION()
	float GetWaveValue(FVector worldPos, float time);
	
	float surfaceLevelHeight = 0;
	
	UPROPERTY(BlueprintType, EditAnywhere)
	// The shader currently requires using 5 waves! No more, no less!
	TArray<FWaveValues> waves;
};