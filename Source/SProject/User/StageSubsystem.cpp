

#include "StageSubsystem.h"
// include Engine
#include "Engine/World.h"
#include "Engine/LocalPlayer.h"
// include Project
#include "Types/FetchDocumentTypes.h"
#include "Table/TableSubsystem.h"
#include "Table/StageTable.h"
#include "UserAccountSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageSubsystem)

DEFINE_LOG_CATEGORY(LogStage)

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

FStage UStageSubsystem::BP_GetStage() const
{
	return GetStage().Get();
}

TSharedRef<FStage> UStageSubsystem::GetStage() const
{
	if(!Stage.IsValid())
	{
		Stage = MakeShared<FStage>();
	}
	return Stage.ToSharedRef();
}

FStageDetailSpec UStageSubsystem::BP_GetStageDetail(int32 Level) const
{
	return GetStageDetail(Level).Get();
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

FTower UStageSubsystem::BP_GetTower(int32 Level, int32 Key) const
{
	return GetTower(Level, Key).Get();
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

//////////////////////////////////////////////////////////////////////////
// UStageSubsystemHelper
//////////////////////////////////////////////////////////////////////////

TSoftObjectPtr<UWorld> UStageSubsystemHelper::GetStageMap(int32 Level)
{
	UTableSubsystem* TableSubsystem = UTableSubsystem::Get();
	if (!IsValid(TableSubsystem))
	{
		UE_LOG(LogStage, Error, TEXT("[LoadStageLevel] TableSubsystem is nullptr."));
		return nullptr;
	}

	auto StageData = TableSubsystem->GetTableData<FStageTableRow>(Level);
	if (nullptr == StageData)
	{
		UE_LOG(LogStage, Error, TEXT("[LoadStageLevel] 테이블에서 Stage 데이터를 찾지 못하였습니다. [Level: %d]"), Level);
		return nullptr;
	}

	auto& MapPtr = StageData->Map;
	if (MapPtr.IsNull())
	{
		UE_LOG(LogStage, Error, TEXT("[LoadStageLevel] Stage 데이터에 Map이 연결되어 있지 않습니다.. [Level: %d]"), Level);
		return nullptr;
	}

	return StageData->Map;
}
