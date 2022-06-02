

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BoatEnums.h"
#include "CrewComp.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCrewSend, int, startSection,int,endSection);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3TEAM9_API UCrewComp : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCrewComp();

	void MoveCrewMemberX(float xDireciton);
	void MoveCrewMemberY(float yDireciton);

	UFUNCTION(BlueprintCallable)
	void AddNewCrew(int amountToIncrease);

	FCrewSend OnSendCrew;

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void MoveCrew(EHealthSectionPosition systemPosDirection, EHealthSectionPosition InvertedSystemPosDirection);

	TMap<EHealthSectionPosition, class UHealthSection*> crewableSections;

	UPROPERTY(EditDefaultsOnly)
	int maxCrewInSystem = 5;

	UPROPERTY(EditDefaultsOnly)
	int crewReserve = 2;

	UPROPERTY(EditDefaultsOnly)
		float repairPercentageIncrease = 0.02f;

	UPROPERTY(EditDefaultsOnly)
		float maxRepairTickTimer = 1.f;
	float currentRepairTime = 0.f;

	bool bMovedX = false;
	bool bMovedY = false;


};
