
#include "CinemachineBrainComponent.h"

UCinemachineBrainComponent::UCinemachineBrainComponent()
{
}

void UCinemachineBrainComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (UpdateMethod == ECinemachineBrainUpdateMethod::ManualUpdate)
	{
        ManualUpdate(DeltaTime);
	}
}

UCameraComponent* UCinemachineBrainComponent::GetOutputCamera()
{
	return nullptr;
}

FVector UCinemachineBrainComponent::DefaultWorldUp() const
{
	return FVector();
}

void UCinemachineBrainComponent::ManualUpdate(float DeltaTime)
{
    
}