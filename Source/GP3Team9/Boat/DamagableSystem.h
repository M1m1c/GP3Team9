#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BoatEnums.h"
#include "DamagableSystem.generated.h"

UINTERFACE(MinimalAPI)
class UDamagableSystem : public UInterface
{
	GENERATED_BODY()
};

class GP3TEAM9_API IDamagableSystem
{
	GENERATED_BODY()
public:
	virtual EHealthSectionPosition GetSectionPosition();
	virtual void DisableSystem();
	virtual void EnableSystem();
	virtual void UpdateCrewCount(int newCrewCount);
};
