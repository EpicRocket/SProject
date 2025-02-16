

#include "StageSubsystem.h"
// include Engine
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"
// include Project
#include "UserLogging.h"
#include "Types/FetchDocumentTypes.h"
#include "Table/TableSubsystem.h"
#include "UserAccountSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageSubsystem)

//////////////////////////////////////////////////////////////////////////
// UStageSubsystem
//////////////////////////////////////////////////////////////////////////

UStageSubsystem* UStageSubsystem::Get(const ULocalPlayer* InLocalPlayer)
{
	if (!IsValid(InLocalPlayer))
	{
		return nullptr;
	}
	return InLocalPlayer->GetSubsystem<UStageSubsystem>();
}

void UStageSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (auto Subsystem = Collection.InitializeDependency<UUserAccountSubsystem>())
	{
		Subsystem->BindUserDocumentMiddleware(this);
	}
}

void UStageSubsystem::Deinitialize()
{
	if (auto Subsystem = UUserAccountSubsystem::Get(GetLocalPlayer()))
	{
		Subsystem->UnbindUserDocumentMiddleware(this);
	}

	Super::Deinitialize();
}

void UStageSubsystem::ApplyUserDocumentChanges(const TSharedRef<FFetchDocument> FetchDocument)
{
	if (FetchDocument->LastStageLevel.IsSet())
	{
		GetLastStageLevel().Get() = FetchDocument->LastStageLevel.GetValue();
	}

	if (FetchDocument->Stages.IsSet())
	{
		TSet<int32> ExistsStage;
		ExistsStage.Reserve(FetchDocument->Stages->Num());
		for (auto const& Detail : FetchDocument->Stages.GetValue())
		{
			ExistsStage.Emplace(Detail.Level);
			GetStage(Detail.Level).Get() = Detail;
		}

		TSet<int32> DeleteStage;
		DeleteStage.Reserve(Stages.Num());
		for (auto& [Level, _] : Stages)
		{
			if (!ExistsStage.Contains(Level))
			{
				DeleteStage.Emplace(Level);
			}
		}

		for (auto& Key : DeleteStage)
		{
			Stages.Remove(Key);
		}
	}
}

int32 UStageSubsystem::K2_GetLastStageLevel() const
{
	return GetLastStageLevel().Get();
}

TSharedRef<int32> UStageSubsystem::GetLastStageLevel() const
{
	if (!LastStageLevel.IsValid())
	{
		LastStageLevel = MakeShared<int32>();
	}
	return LastStageLevel.ToSharedRef();
}

FStage UStageSubsystem::K2_GetLastStage() const
{
	return GetLastStage().Get();
}

TSharedRef<FStage> UStageSubsystem::GetLastStage() const
{
	return GetStage(GetLastStageLevel().Get());
}

FStage UStageSubsystem::K2_GetStage(int32 Level) const
{
	return GetStage(Level).Get();
}

TSharedRef<FStage> UStageSubsystem::GetStage(int32 Level) const
{
	if (!Stages.Contains(Level))
	{
		auto NewStage = MakeShared<FStage>();
		NewStage->Level = Level;
		Stages.Emplace(Level, NewStage);
	}
	return Stages[Level].ToSharedRef();
}
