
#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ScriptInterface.h"

#include "GTeamSubsystem.generated.h"

class UDataTable;
class APlayerController;
class IGTeamAgent;
struct FGenericTeamId;
struct FGTeamTracker;
struct FGTeam;
struct FGRelationshipInForceTableRow;
struct FGForcesRelationshipTableRow;

namespace ETeamAttitude
{
	enum Type : int;
}

GGAMECORE_API DECLARE_LOG_CATEGORY_EXTERN(LogGTeam, Log, All);

UCLASS(Abstract, Config = Game)
class GGAMECORE_API UGTeamSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

	using FTeamIndex = uint8;
	using FTeamForceIndex = uint8;

protected:
	virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

public:
	UFUNCTION(BlueprintCallable)
	void RegisterTeams(const TArray<FGTeamTracker>& TeamTrackers, const TArray<FGRelationshipInForceTableRow>& RelationshipInForceTableRows, const TArray<FGForcesRelationshipTableRow>& ForcesRelationshipTableRows);

	UFUNCTION(BlueprintCallable)
	void UnregisterTeams();

	UFUNCTION(BlueprintPure)
	FGTeamTracker GetTeamTracker(FGenericTeamId TeamID) const;

	UFUNCTION(BlueprintPure)
	TEnumAsByte<ETeamAttitude::Type> GetTeamAttitudeTowards(uint8 Source, uint8 Target) const;

	UFUNCTION(BlueprintPure)
	TEnumAsByte<ETeamAttitude::Type> GetAgentAttitudeTowards(TScriptInterface<IGTeamAgent> Source, TScriptInterface<IGTeamAgent> Target) const;

protected:
	virtual void OnRegisterTeams() {}
	virtual void OnUnregisterTeams() {}

protected:
	TMap<FTeamIndex, TSharedPtr<FGTeam>> Teams;
	TMap<FTeamForceIndex, TArray<TSharedPtr<FGTeam>>> Forces;
	
};

UCLASS()
class GGAMECORE_API UGTeamHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (ReturnDisplayName = "Success"))
	static bool LoadTeamLoadAsset(TSoftObjectPtr<UGTeamLoadDataAsset> DataAsset, TArray<FGTeamTracker>& TeamTrackers, TArray<FGRelationshipInForceTableRow>& RelationshipInForceTableRows, TArray<FGForcesRelationshipTableRow>&  ForcesRelationshipTableRows);

	UFUNCTION(BlueprintPure, meta = (ReturnDisplayName = "Success"))
	static bool IsTeamAgentOwner(APlayerController* PC, TScriptInterface<IGTeamAgent> TeamAgent);

};
