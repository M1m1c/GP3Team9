
#pragma once

#include "CoreMinimal.h"
#include "GP3ForceFeedbackComp.generated.h"



UCLASS(Blueprintable)
class GP3TEAM9_API UGP3ForceFeedbackComp : public UActorComponent
{
	GENERATED_BODY()

public:
	// UPROPERTY(EditAnywhere, BlueprintReadWrite)
	// TArray<UForceFeedbackEffect*> Effects;
	
	UFUNCTION(BlueprintCallable)
	void PlayForceFeedbackEffect(UForceFeedbackEffect* Effect)	{
		GetWorld()->GetFirstPlayerController()->ClientPlayForceFeedback(Effect);
	}

	
};
