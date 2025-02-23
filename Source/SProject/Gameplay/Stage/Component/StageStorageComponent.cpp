
#include "StageStorageComponent.h"

// include Project
#include "Types/StageTypes.h"
#include "Gameplay/Stage/StageTableRepository.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageStorageComponent)

FGErrorInfo UStageStorageComponent::WaitForInitialize(FLatentActionInfo LatentInfo)
{
	return FGErrorInfo();
}

FStage UStageStorageComponent::GetLastStage() const
{
	return FStage();
}
