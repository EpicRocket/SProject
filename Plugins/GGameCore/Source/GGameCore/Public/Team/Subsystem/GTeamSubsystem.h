
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
    void RegisterTeams(const TArray<FGTeamTracker>& TeamTrackers);

    UFUNCTION(BlueprintPure)
	FGTeamTracker GetTeamTracker(FGenericTeamId TeamID) const;

    UFUNCTION(BlueprintPure)
	TEnumAsByte<ETeamAttitude::Type> GetTeamAttitudeTowards(const FGenericTeamId& TeamA, const FGenericTeamId& TeamB) const;

private:
    TMap<FTeamIndex, TSharedPtr<FGTeam>> Teams;
	TMap<FTeamForceIndex, TArray<TSharedPtr<FGTeam>>> Forces;
    
};

UCLASS()
class GGAMECORE_API UGTeamHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "팀", meta = (ReturnDisplayName = "Success"))
    static bool LoadTeamTableRows(UDataTable* Table, TArray<FGTeamTracker>& TeamTrackers);

    UFUNCTION(BlueprintPure, Category = "팀", meta = (ReturnDisplayName = "Success"))
    static bool IsTeamAgentOwner(APlayerController* PC, TScriptInterface<IGTeamAgent> TeamAgent);
};
