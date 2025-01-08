
#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Team/GTeamTypes.h"

#include "GTeamSubsystem.generated.h"

class UDataTable;
struct FGenericTeamId;

GGAMECORE_API DECLARE_LOG_CATEGORY_EXTERN(LogGTeam, Log, All);

UCLASS(Abstract, Config = Game)
class GGAMECORE_API UGTeamSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

protected:
    virtual bool DoesSupportWorldType(const EWorldType::Type WorldType) const override;

public:
    UFUNCTION(BlueprintCallable)
    void RegisterTeams(const TArray<FGTeamTracker>& InTeams, TArray<FGTeamForce> InForces);

    UFUNCTION(BlueprintPure)
	FGTeamTracker GetTeamTracker(FGenericTeamId TeamID) const;

private:
    TMap<uint8, TSharedPtr<FGTeamTracker>> Teams;
	TMap<uint8, TArray<uint8>> TeamForces;
};

UCLASS()
class GGAMECORE_API UGTeamHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Team", meta = (ReturnDisplayName = "Success"))
	static bool LoadTeamForceTableRows(UDataTable* Table, TArray<FGTeamForce>& Forces);

    UFUNCTION(BlueprintCallable, Category = "Team", meta = (ReturnDisplayName = "Success"))
    static bool LoadTeamTableRows(UDataTable* Table, TArray<FGTeamTracker>& Teams);
};
