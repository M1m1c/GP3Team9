#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BoatEnums.h"
#include "DamagableSystem.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamagableSystem : public UInterface
{
	GENERATED_BODY()
};

class GP3TEAM9_API IDamagableSystem
{
	GENERATED_BODY()
		//TODO add function for setting crew 
		// and add what crew does to modify system in the class that implements this
		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual EHealthSectionPosition GetSectionPosition();
	virtual void DisableSystem();
	virtual void EnableSystem();
	virtual void UpdateCrewCount(int newCrewCount);
	
protected:

	
};
