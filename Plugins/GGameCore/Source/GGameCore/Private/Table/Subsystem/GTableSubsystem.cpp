// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Table/Subsystem/GTableSubsystem.h"
// include Engine
#include "Engine/Engine.h"
#include "UObject/SoftObjectPath.h"
#include "Misc/Paths.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetRegistry/IAssetRegistry.h"
#include "AssetRegistry/ARFilter.h"
#include "AssetRegistry/AssetData.h"
// include GameCore
#include "Table/GTableSettings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GTableSubsystem)

DEFINE_LOG_CATEGORY(LogGameTable)

/* static */UGTableSubsystem* UGTableSubsystem::Get()
{
	if (GEngine == NULL)
	{
		return nullptr;
	}
	return GEngine->GetEngineSubsystem<UGTableSubsystem>();
}

bool UGTableSubsystem::LoadTable()
{
	auto& RootTableDir = GetDefault<UGTableSettings>()->RootTableDir;
	if (!FPaths::DirectoryExists(RootTableDir.Path))
	{
		UE_LOG(LogGameTable, Error, TEXT("RootTableDir is not exist: %s"), *RootTableDir.Path);
		return false;
	}

	Tables.Empty();

	auto& AssetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
	auto& AssetRegister = AssetRegistryModule.Get();

	FARFilter Filter;
	Filter.ClassPaths.Emplace(UDataTable::StaticClass()->GetPathName());
	Filter.PackagePaths.Emplace(FName(*(TEXT("/Game") / RootTableDir.Path)));
	Filter.bRecursivePaths = true;

	TArray<FAssetData> Assets;
	AssetRegister.GetAssets(Filter, Assets, true);

	for (auto const& Asset : Assets)
	{
		UDataTable* DataTable = Cast<UDataTable>(Asset.GetAsset());
		if (!DataTable)
		{
			continue;
		}

		auto const* ScriptStruct = DataTable->GetRowStruct();
		if (!ScriptStruct)
		{
			continue;
		}

		Tables.Emplace(GetTypeHash(ScriptStruct), DataTable);
	}

	CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);

	TableLoadCompleted.Broadcast();
	return true;
}
