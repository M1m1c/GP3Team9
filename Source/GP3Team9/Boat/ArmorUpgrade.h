
#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BoatUpgrade.h"
#include "ArmorUpgrade.generated.h"

UENUM(Blueprintable)
enum class EArmorTier : uint8
{
	None,
	Tier1,
	Tier2,
	Tier3
};

UCLASS(Blueprintable)
class GP3TEAM9_API UArmorUpgrade : public UObject, public IBoatUpgrade
{
	GENERATED_BODY()

public:

	virtual EUpgradeType GetUpgradeType() override;

	UPROPERTY(EditDefaultsOnly)
		float ArmorValue = 25.f;

	UPROPERTY(EditDefaultsOnly)
		EArmorTier ArmorTier = EArmorTier::Tier1;
};
