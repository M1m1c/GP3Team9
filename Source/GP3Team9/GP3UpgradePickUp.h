
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GP3UpgradePickUp.generated.h"

UCLASS()
class GP3TEAM9_API AGP3UpgradePickUp : public AActor
{
	GENERATED_BODY()

public:
	AGP3UpgradePickUp();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
};
