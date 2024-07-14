
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Templates/SharedPointer.h"
#include "Interface/UserDocumentMiddleware.h"

#include "StageSubsystem.generated.h"

struct FStage;
struct FStageDetail;
struct FTower;

USTRUCT(BlueprintType)
struct SPROJECT_API FStageDetailSpec
{
    GENERATED_BODY()

    int32 Level = 0;

    TMap<int32, TSharedPtr<FTower>> Towers;
};

/*
 * 유저의 스테이지 별 정보를 관리하는 서브시스템
 */
UCLASS()
class SPROJECT_API UStageSubsystem : public ULocalPlayerSubsystem, public IUserDocumentMiddleware
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

    UFUNCTION(BlueprintCallable, Category = "스테이지", meta = (ShortToolTip = "유저가 달성한 최대 스테이지 레벨"))
    int32 GetUserStageLevel() const;

    UFUNCTION(BlueprintCallable, Category = "스테이지")
    TArray<FTower> GetStageTowers(int32 Level) const;

private:
    TSharedRef<FStage> GetStage() const;

    TSharedRef<FStageDetailSpec> GetStageDetail(int32 Level) const;

    TSharedRef<FTower> GetTower(int32 Level, int32 Key) const;

private:
    mutable TSharedPtr<FStage> Stage;

    mutable TMap<int32, TSharedPtr<FStageDetailSpec>> StageDetails;
};
