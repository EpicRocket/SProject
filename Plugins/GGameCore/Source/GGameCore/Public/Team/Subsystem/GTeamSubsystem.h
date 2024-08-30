
#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Team/GTeamTypes.h"

#include "GTeamSubsystem.generated.h"

class UDataTable;

UCLASS(Abstract)
class GGAMECORE_API UGTeamSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void RegisterTeams(const TArray<FGTeamTracker>& InTeams);

private:
    UPROPERTY(Transient)
    TMap<uint8, FGTeamTracker> Teams;
};

UCLASS()
class GGAMECORE_API UGTeamHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Team")
    static void LoadTeamTableRows(UDataTable* Table, TArray<FGTeamTracker>& Teams);
};
