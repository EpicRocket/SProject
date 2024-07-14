

#include "StageSubsystem.h"
// include Engine
#include "Engine/LocalPlayer.h"
// include Project
#include "Types/FetchDocumentTypes.h"
#include "UserAccountSubsystem.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(StageSubsystem)

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
	if (FetchDocument->Stage.IsSet())
	{
		GetStage().Get() = FetchDocument->Stage.GetValue();
	}

	if (FetchDocument->StageDetails.IsSet())
	{
		for (auto const& Detail : FetchDocument->StageDetails.GetValue())
		{
			GetStageDetail(Detail.Level).Get().Level = Detail.Level;

			// HACK: 현재는 모든 타워 정보를 덮어씌우지만, 추후에는 변경된 타워 정보만 업데이트 해야함
			TSet<int32> TowerKeys; TowerKeys.Reserve(Detail.Towers.Num());
			for (auto const& Tower : Detail.Towers)
			{
				GetTower(Detail.Level, Tower.Key).Get() = Tower;
				TowerKeys.Emplace(Tower.Key);
			}

			for (auto& [Key, Tower] : GetStageDetail(Detail.Level).Get().Towers)
			{
				if (!TowerKeys.Contains(Key))
				{
					Tower->Level = 0;
					Tower->Location = INDEX_NONE;
				}
			}
		}
	}
}

int32 UStageSubsystem::GetUserStageLevel() const
{
	return GetStage().Get().Level;
}

TArray<FTower> UStageSubsystem::GetStageTowers(int32 Level) const
{
	auto const& StageDetail = GetStageDetail(Level).Get();
	
	TArray<FTower> Result;
	Result.Reserve(StageDetail.Towers.Num());
	for (auto const& Tower : StageDetail.Towers)
	{
		Result.Emplace(*Tower.Value);
	}

	return Result;
}

TSharedRef<FStage> UStageSubsystem::GetStage() const
{
	if(!Stage.IsValid())
	{
		Stage = MakeShared<FStage>();
	}
	return Stage.ToSharedRef();
}

TSharedRef<FStageDetailSpec> UStageSubsystem::GetStageDetail(int32 Level) const
{
	if (!StageDetails.Contains(Level))
	{
		auto Spec = MakeShared<FStageDetailSpec>();
		Spec->Level = Level;
		StageDetails.Emplace(Level, Spec);
	}
	return StageDetails[Level].ToSharedRef();
}

TSharedRef<FTower> UStageSubsystem::GetTower(int32 Level, int32 Key) const
{
	auto& StageDetail = GetStageDetail(Level).Get();
	if (!StageDetail.Towers.Contains(Key))
	{
		auto Tower = MakeShared<FTower>();
		Tower->Key = Key;
		StageDetail.Towers.Emplace(Key, Tower);
	}
	return StageDetail.Towers[Key].ToSharedRef();
}
