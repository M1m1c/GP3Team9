#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BoatUpgrade.generated.h"

UENUM(Blueprintable)
enum class EUpgradeType : uint8
{
	None,
	Gun,
	Engine,
	Armor,
	Boat,
	Icebreaker
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UBoatUpgrade : public UInterface
{
	GENERATED_BODY()

};

class GP3TEAM9_API IBoatUpgrade
{
	GENERATED_BODY()
		// and add what crew does to modify system in the class that implements this
		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual EUpgradeType GetUpgradeType();

protected:


};

