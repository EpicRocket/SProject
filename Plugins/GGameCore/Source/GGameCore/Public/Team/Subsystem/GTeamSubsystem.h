
#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ScriptInterface.h"

#include "GTeamSubsystem.generated.h"

class APlayerController;
class IGTeamAgent;
class UGTeam;
struct FGenericTeamId;
struct FGTeamTracker;
struct FGRelationshipInForceTableRow;
struct FGForcesRelationshipTableRow;
struct FGErrorInfo;

namespace ETeamAttitude
{
	enum Type : int;
}
class AGameplayPlayer;
class UGTeam;

GGAMECORE_API DECLARE_LOG_CATEGORY_EXTERN(LogGTeam, Log, All);

UCLASS(Abstract, Config = Game, Category = "Team", ClassGroup = "Team")
class GGAMECORE_API UGTeamSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

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

	UFUNCTION(BlueprintCallable)
	uint8 IssusePlayerTeamID(APlayerController* PC);

	UFUNCTION(BlueprintCallable)
	AGameplayPlayer* GetPlayer(uint8 ID) const;

protected:
	virtual void OnRegisterTeams() {}
	virtual void OnUnregisterTeams() {}

private:
	void Empty();

protected:
	UPROPERTY(Transient)
	TMap<uint8/*TeamIndex*/, UGTeam*> Teams;
	TMultiMap<uint8/*ForceIndex*/, TWeakObjectPtr<UGTeam>> Forces;
	
	UPROPERTY(Transient)
	TMap<uint8, AGameplayPlayer*> Players;

};

UCLASS()
class GGAMECORE_API UGTeamHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, meta = (ReturnDisplayName = "Success"))
	static FGErrorInfo LoadTeamLoadAsset(TSoftObjectPtr<UGTeamLoadDataAsset> DataAsset, TArray<FGTeamTracker>& TeamTrackers, TArray<FGRelationshipInForceTableRow>& RelationshipInForceTableRows, TArray<FGForcesRelationshipTableRow>&  ForcesRelationshipTableRows);

	UFUNCTION(BlueprintPure, meta = (ReturnDisplayName = "Success"))
	static bool IsTeamAgentOwner(APlayerController* PC, TScriptInterface<IGTeamAgent> TeamAgent);

};
