
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SharedPointer.h"
#include "UserDocumentMiddleware.h"

#include "StageSubsystem.generated.h"

class UWorld;

struct FStage;
struct FTower;

/*
 * 유저의 스테이지 별 정보를 관리하는 서브시스템
 */
UCLASS()
class MY_API UStageSubsystem : public ULocalPlayerSubsystem, public IUserDocumentMiddleware
{
    GENERATED_BODY()

public:
    static UStageSubsystem* Get(const ULocalPlayer* InLocalPlayer);

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // IUserDocumentMiddleware
    virtual void ApplyUserDocumentChanges(const TSharedRef<FFetchDocument> FetchDocument) override;
    // ~IUserDocumentMiddleware

    UFUNCTION(BlueprintCallable, Category = "스테이지", meta = (DisplayName = "GetLastStageLevel"))
    int32 K2_GetLastStageLevel() const;
    TSharedRef<int32> GetLastStageLevel() const;

    UFUNCTION(BlueprintCallable, Category = "스테이지", meta = (DisplayName = "GetLastStage"))
    FStage K2_GetLastStage() const;
    TSharedRef<FStage> GetLastStage() const;
    
    UFUNCTION(BlueprintCallable, Category = "스테이지", meta = (DisplayName = "GetStageLevel"))
    FStage K2_GetStage(int32 Level) const;
    TSharedRef<FStage> GetStage(int32 Level) const;

private:
    mutable TSharedPtr<int32> LastStageLevel;

    mutable TMap<int32/*Level*/, TSharedPtr<FStage>> Stages;
};


UCLASS()
class MY_API UStageSubsystemHelper : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
};