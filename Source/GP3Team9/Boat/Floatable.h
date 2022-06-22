
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Floatable.generated.h"


UINTERFACE(MinimalAPI)
class UFloatable : public UInterface
{
	GENERATED_BODY()

};

class GP3TEAM9_API IFloatable
{
	GENERATED_BODY()
		
public:
	virtual UStaticMeshComponent* GetFloatBody();

};