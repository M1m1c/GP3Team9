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
	FVector waveDirection;
	UPROPERTY(EditAnywhere)
	float waveLength;
	UPROPERTY(EditAnywhere)
	float waveSpeed;
	UPROPERTY(EditAnywhere)
	float waveAmplitude;
	UPROPERTY(EditAnywhere)
	float gerstnerFactor;
};

UCLASS()
class GP3TEAM9_API UWaveHeightmap : public UDataAsset
{
	GENERATED_BODY()

public:
	UFUNCTION()
	float GetWaveValue(FVector worldPos, float time);

	UPROPERTY(BlueprintType, EditAnywhere)
	TArray<FWaveValues> waves;
};