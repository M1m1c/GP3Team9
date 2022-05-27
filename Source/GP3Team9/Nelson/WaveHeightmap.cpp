// Fill out your copyright notice in the Description page of Project Settings.


#include "WaveHeightmap.h"


float UWaveHeightmap::GetWaveValue(FVector worldPos, float time)
{
	if(waves.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Wave array size is 0 - returning 0 height on all positions"));
		return 0;
	}
	
	float waveHeight = 0;

	for (int i = 0; i < waves.Num(); i++)
	{
		FWaveValues wave = waves[i];
		float waveNumber = 1.0f / wave.waveLength;

		// make an SDF with worldPos and waveDir, scale it to waveNumber (inverse waveLength)
		float waveFront = FVector::DotProduct(wave.waveDirection.GetSafeNormal(), worldPos) * waveNumber;

		// Apply the time offset to the SDF
		float transformedTime = waveFront - time * wave.waveSpeed;

		// Add a sine wave to the transformed time to make the gerstner shape for the wave
		float gerTime = transformedTime + wave.gerstnerFactor * sin(transformedTime);

		// applying amplitude
		waveHeight += wave.waveAmplitude * cos(gerTime);
	}
	
	return waveHeight;
}
