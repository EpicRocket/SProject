

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InputActionValue.h"
#include "CinemachineInputAxisProviderInterface.generated.h"

UINTERFACE(BlueprintType)
class UCinemachineInputAxisProviderInterface : public UInterface
{
	GENERATED_BODY()
};

class ICinemachineInputAxisProviderInterface
{
	GENERATED_BODY()

public:
	virtual float GetInputAxisValue(EAxis::Type ProviderAxis) = 0;
};
