// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UpgradeManagerComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3TEAM9_API UUpgradeManagerComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUpgradeManagerComp();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void UpgradeArmorPath(class IBoatUpgrade* newUpgrade);

	TArray<class IBoatUpgrade*> heldUpgrades;

	class ABoatPawn* owner;

	bool bInitalized = false;
public:	
	
	void ApplyUpgrade(class IBoatUpgrade* newUpgrade);

		
};
