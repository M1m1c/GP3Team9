

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

	void Initalize();

	void MoveCrewMemberX(float xDireciton);
	void MoveCrewMemberY(float yDireciton);

	void EnableCrewLock();
	void DisableCrewLock();

	UFUNCTION(BlueprintCallable)
	void AddNewCrew(int amountToIncrease);

	void SetCrewCount(int count);

	UFUNCTION(BlueprintCallable)
	int GetCrewCount();

	FCrewSend OnSendCrew;

protected:

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void MoveCrew(EHealthSectionPosition systemPosDirection, EHealthSectionPosition InvertedSystemPosDirection);

	TMap<EHealthSectionPosition, class UHealthSection*> crewableSections;

	UPROPERTY(EditDefaultsOnly)
	int crewRequiredToLevel = 5;

	UPROPERTY(EditDefaultsOnly)
	int maxCrewInSystem = 10;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int crewReserve = 2;

	int totalCrew = 2;

	UPROPERTY(EditAnywhere)
		//!!!only set the numbers on these on enemy ships!!!
		//this is how many crew each section will start with
		TMap<EHealthSectionPosition,int> startingCrewInSections 
	{
			{EHealthSectionPosition::Front,0},
			{EHealthSectionPosition::Back,0},
			{EHealthSectionPosition::Left,0},
			{EHealthSectionPosition::Right,0},
	};

	UPROPERTY(EditDefaultsOnly)
		float repairPercentageIncrease = 0.02f;

	UPROPERTY(EditDefaultsOnly)
		float maxRepairTickTimer = 1.f;
	float currentRepairTime = 0.f;

	bool bMovedX = false;
	bool bMovedY = false;

	class ABoatPawn* boatPawn;
	bool bCrewLock = true;
};
