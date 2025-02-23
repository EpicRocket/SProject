
#include "StageStorageComponent.h"
// include Engine
#include "Engine/World.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/LatentActionManager.h"
#include "Kismet/KismetSystemLibrary.h"
// include GameCore
#include "Error/GError.h"
#include "Core/Action/GGameComponentLoadAction.h"
// include Project
#include "Types/StageTypes.h"
#include "Gameplay/Stage/StageTableRepository.h"
#include "User/StageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageStorageComponent)

FGErrorInfo UStageStorageComponent::WaitForInitialize(FLatentActionInfo LatentInfo)
{
	auto World = GetWorld();
	if (!World)
	{
		UKismetSystemLibrary::DelayUntilNextTick(World, LatentInfo);
		return GameCore::Throw(GameErr::WORLD_INVALID);
	}

	FGErrorInfo ErrorInfo;

	auto OnComplete = [this, &ErrorInfo]()
		{
			auto StageSubsystem = UStageSubsystem::Get(GetOwningLocalPlayer());
			if (!StageSubsystem)
			{
				ErrorInfo = GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("StageSubsystem"));
				return;
			}

			auto LastStage = StageSubsystem->GetLastStage();
			LastStageLevel = LastStage->Level;
			Stages.Emplace(LastStage->Level, LastStage);
		};

	auto NewAction = new FGGameComponentLoadAction(LatentInfo, GetWorld(), this, OnComplete, [&ErrorInfo](FGErrorInfo Err) {ErrorInfo = Err; });
	World->GetLatentActionManager().AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID, NewAction);

	return ErrorInfo;
}

FStage UStageStorageComponent::GetLastStage() const
{
	auto Iter = Stages.Find(LastStageLevel);
	if (Iter)
	{
		return **Iter;
	}
	return FStage{};
}
