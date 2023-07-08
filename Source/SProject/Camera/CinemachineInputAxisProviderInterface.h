

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputActionValue.h"
#include "CinemachineInputAxisProviderInterface.generated.h"

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCinemachineInputAxisProviderInterface : public UInterface
{
	GENERATED_BODY()
};

class ICinemachineInputAxisProviderInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual float GetInputValue(EAxis::Type ProviderAxis)
	{
		return 0.0F;
	}
};
