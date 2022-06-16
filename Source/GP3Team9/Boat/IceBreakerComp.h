
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "IceBreakerComp.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3TEAM9_API UIceBreakerComp : public UActorComponent
{
	GENERATED_BODY()

public:	
	
	UIceBreakerComp();
	bool IsIceBreakerComplete();
	void IncreaseSegments(int amountToAdd);
	int GetSegementCount();
protected:

	virtual void BeginPlay() override;

	int currentSegments = 0;
	
	UPROPERTY(EditAnywhere)
		int segmentsRequierd = 4;

	class UStaticMeshComponent* icebreakerMesh;
};
