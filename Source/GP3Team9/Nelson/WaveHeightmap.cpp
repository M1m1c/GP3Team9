// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveHeightmap.h"

float UWaveHeightmap::GetWaveValue(FVector worldPos, float time)
{
	if(waves.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wave array size is 0 - returning 0 height on all positions"));
		return 0;
	}
	
	float waveHeight = surfaceLevelHeight;
	float twoPi = 6.28318531f;
	float onePlusEpsilon = 1.1f;
	// float hardcodedTimeDelay = - 0.5f;

	for (int i = 0; i < waves.Num(); i++)
	{
		FWaveValues wave = waves[i];
		// wave number to normalize coordinates into world space
		float waveNumber = twoPi / wave.waveLength;

		// Fake Gerstner Factor to make sharper crests
		float steepnessFactor = onePlusEpsilon - wave.waveSteepness;

		// Sample position before time
		float samplingTemp = FVector::DotProduct(wave.waveDirection.GetSafeNormal(), worldPos) * waveNumber;
		
		// Sample position after time
		float samplePos = samplingTemp - wave.waveSpeed * time; // + hardcodedTimeDelay;

		// samplePos = FMath::Fmod(samplePos,twoPi);

		// Tangent of samplePos
		float tanVal = FMath::Tan(samplePos);

		// Denominator
		float denominatorVal = 1.0f + (tanVal * tanVal/steepnessFactor);

		// applying amplitude
		waveHeight += wave.waveAmplitude / denominatorVal;
	}
	
	return waveHeight;
}