// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WaveController.generated.h"

UCLASS()
class GP3TEAM9_API AWaveController : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	class UWaveHeightmap* WaveHeightMapAsset;

public:	
	// Sets default values for this actor's properties
	AWaveController();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UMaterialInstanceDynamic* WavesMaterialInstance;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
