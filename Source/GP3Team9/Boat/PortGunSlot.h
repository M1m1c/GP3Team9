
#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "BoatEnums.h"
#include "PortGunSlot.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3TEAM9_API UPortGunSlot : public USceneComponent
{
	GENERATED_BODY()

public:	
	UPortGunSlot();


	UPROPERTY(EditAnywhere)
		EGunSlotPosition myPosition;

protected:
	virtual void BeginPlay() override;
};
