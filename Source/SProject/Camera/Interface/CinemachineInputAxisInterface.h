

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Camera/Core/CinemachineInputAxis.h"
#include "CinemachineInputAxisInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCinemachineInputAxisResetEvent);

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCinemachineInputAxisInterface : public UInterface
{
	GENERATED_BODY()
};

class ICinemachineInputAxisInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Input")
	virtual void ProcessInput(FGameplayTag InutAxisTag, float InputValue, float DeltaTime) = 0;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Input")
	virtual void ResetInput(FGameplayTagContainer InputAxisTagContainer) = 0;

	UFUNCTION()
	virtual void OnCancelInputRecentering(FGameplayTag InputAxisTag) = 0;
};
