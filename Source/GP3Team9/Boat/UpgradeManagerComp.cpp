

#include "UpgradeManagerComp.h"


#include "BoatPawn.h"
#include "BoatUpgrade.h"
#include "ArmorUpgrade.h"
#include "HealthComp.h"

UUpgradeManagerComp::UUpgradeManagerComp()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UUpgradeManagerComp::BeginPlay()
{
	Super::BeginPlay();	
	owner = Cast<ABoatPawn>(GetOwner());
	if (!ensure(owner)) { return; }
	bInitalized = true;
}

void UUpgradeManagerComp::ApplyUpgrade(IBoatUpgrade* newUpgrade)
{
	if (!bInitalized) { return; }
	auto upgradeType = newUpgrade->GetUpgradeType();

	switch (upgradeType)
	{
	case EUpgradeType::None:
		break;
	case EUpgradeType::Gun:
		//TODO tell the gun driver to delete its weapons and spawn upgrade weapons.
		break;
	case EUpgradeType::Engine:
		//TODO tell movement comp to unlock highest gear
		break;
	case EUpgradeType::Armor:
		//TODO tell the helth comp to apply armor to all health secitons and spawn armor mesh
		UpgradeArmorPath(newUpgrade);
		break;
	case EUpgradeType::Boat:
		//TODO replace pawn and send held upgrades to it.
		break;
	case EUpgradeType::Icebreaker:
		//TODO attach the icebreaker mesh and enable ramming damage and icebreaking
		break;
	default:
		break;
	}

	heldUpgrades.Add(newUpgrade);
}

void UUpgradeManagerComp::UpgradeArmorPath(IBoatUpgrade* newUpgrade)
{
	auto armorUpgrade = Cast<UArmorUpgrade>(newUpgrade);
	owner->healthComp->UpgradeArmor(armorUpgrade->ArmorValue);
	switch (armorUpgrade->ArmorTier)
	{
	case EArmorTier::Tier1:
		owner->ArmorTier1Graphics->bHiddenInGame = false;
		break;
	case EArmorTier::Tier2:
		owner->ArmorTier2Graphics->bHiddenInGame = false;
		break;
	case EArmorTier::Tier3:
		owner->ArmorTier3Graphics->bHiddenInGame = false;
		break;
	default:
		break;
	}
}

