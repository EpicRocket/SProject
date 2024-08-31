
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "UserDocumentMiddleware.h"

#include "DomainSubsystem.generated.h"

enum class EDomainType : uint8;

struct FFetchDocument;
struct FDomainBuilding;
struct FTowerTableRow;

class ULocalPlayer;

/*
 * 유저의 영지 시스템을 관리하는 서브시스템 클래스
 */
UCLASS()
class MY_API UDomainSubsystem : public ULocalPlayerSubsystem, public IUserDocumentMiddleware
{
    GENERATED_BODY()

public:
    static UDomainSubsystem* Get(const ULocalPlayer* InLocalPlayer);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // IUserDocumentMiddleware
    virtual void ApplyUserDocumentChanges(const TSharedRef<FFetchDocument> FetchDocument) override { /*구현 해야함*/ }
    // ~IUserDocumentMiddleware

public:
    UFUNCTION(BlueprintCallable, Category = "타워", meta = (ShortTooltip = "건설 가능한 타워인지 판별 합니다."))
    bool IsBuildableTower(const FTowerTableRow& TowerData) const;

private:
    TMap<EDomainType, TSharedPtr<FDomainBuilding>> DomainBuildings;
};
