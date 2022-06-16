
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Floatable.generated.h"


// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UFloatable : public UInterface
{
	GENERATED_BODY()

};

class GP3TEAM9_API IFloatable
{
	GENERATED_BODY()
		// and add what crew does to modify system in the class that implements this
		// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual UStaticMeshComponent* GetFloatBody();

protected:


};