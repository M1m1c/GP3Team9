
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BoatEnums.generated.h"

UENUM(Blueprintable)
enum class EGunSlotPosition : uint8
{
	None,
	Left,
	Right,
	Swivel
};

UENUM(Blueprintable)
enum class EHealthSectionPosition : uint8
{
	None,
	Left,
	Right,
	Front,
	Back
};

UCLASS(Abstract)
class GP3TEAM9_API UBoatEnums : public UDataAsset
{
	GENERATED_BODY()
	
};
