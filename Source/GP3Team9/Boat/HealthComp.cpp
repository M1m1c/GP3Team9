
#include "HealthComp.h"

UHealthComp::UHealthComp()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UHealthComp::BeginPlay()
{
	Super::BeginPlay();
	
}


void UHealthComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

