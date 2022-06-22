
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
	UFUNCTION(BlueprintCallable)
	int GetSegementCount();

	void Initalize();
protected:

	virtual void BeginPlay() override;

	int currentSegments = 0;
	
	UPROPERTY(EditAnywhere)
		int segmentsRequierd = 4;

	UPROPERTY(EditAnywhere)
		bool bStartWithIceBreaker = false;

	class UStaticMeshComponent* icebreakerMesh;
	bool bInitalize = false;
};
