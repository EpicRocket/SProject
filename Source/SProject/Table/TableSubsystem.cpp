
#include "TableSubsystem.h"
// include Engine
#include "Engine/Engine.h"
#include "UObject/SoftObjectPath.h"
#include "Misc/Paths.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "AssetRegistry/ARFilter.h"
#include "AssetRegistry/AssetData.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(TableSubsystem)

DEFINE_LOG_CATEGORY(LogTable)

UTableSubsystem* UTableSubsystem::Get()
{
	if (GEngine == NULL)
	{
		return nullptr;
	}
	return GEngine->GetEngineSubsystem<UTableSubsystem>();
}

void UTableSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

bool UTableSubsystem::LoadTable()
{
	// 폴더 체크
	FDirectoryPath RootTablePath;
	RootTablePath.Path = FPaths::ProjectContentDir() / RootTableRelativePath;

	if (!FPaths::DirectoryExists(RootTablePath.Path))
	{
		UE_LOG(LogTable, Error, TEXT("RootTablePath is not exist: %s"), *RootTablePath.Path);
		return false;
	}

	Tables.Empty();

	auto& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	auto& AssetRegister = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassPaths.Emplace(UDataTable::StaticClass()->GetPathName());
	Filter.PackagePaths.Emplace(FName(*(TEXT("/Game") / RootTableRelativePath)));
	Filter.bRecursivePaths = true;

	TArray<FAssetData> Assets;
	AssetRegister.GetAssets(Filter, Assets, true);

	for (auto const& Asset : Assets)
	{
		UDataTable* DataTable = Cast<UDataTable>(Asset.GetAsset());
		if (!DataTable)
		{
			UE_LOG(LogTable, Warning, TEXT("에셋 로드 실패 [%s]"), *Asset.AssetName.ToString());
			continue;
		}

		auto const* ScriptStruct = DataTable->GetRowStruct();
		if (!ScriptStruct)
		{
			UE_LOG(LogTable, Warning, TEXT("스크립트 구조체 로드 실패 [%s]"), *Asset.AssetName.ToString());
			continue;
		}

		UE_LOG(LogTable, Verbose, TEXT("에셋 로그 성공 [%s]"), *Asset.AssetName.ToString())

		Tables.Emplace(GetTypeHash(ScriptStruct), DataTable);
	}

	CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

	return true;
}
