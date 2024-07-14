
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Templates/SharedPointer.h"
#include "Interface/UserDocumentMiddleware.h"

#include "InventorySubsystem.generated.h"

struct FFetchDocument;
struct FItem;

/*
 * 유저의 자원을 관리하는 시스템
 */
UCLASS()
class SPROJECT_API UInventorySubsystem : public ULocalPlayerSubsystem, public IUserDocumentMiddleware
{
    GENERATED_BODY()
    
public:
    static UInventorySubsystem* Get(const ULocalPlayer* InLocalPlayer);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // IUserDocumentMiddleware
    virtual void ApplyUserDocumentChanges(const TSharedRef<FFetchDocument> FetchDocument) override;
    // ~IUserDocumentMiddleware

    UFUNCTION(BlueprintCallable, Category = "인벤토리")
    int64 GetGold() const;

    UFUNCTION(BlueprintCallable, Category = "인벤토리")
    int32 GetCash() const;

    UFUNCTION(BlueprintCallable, Category = "인벤토리", meta = (ShortToolTip = "인벤토리에서 아이템을 찾습니다."))
    FItem Get(int32 Key) const;

private:
    TSharedRef<FItem> GetItem(int32 Key) const;

private:
    int64 Gold = 0;

    int32 Cash = 0;

    mutable TMap<int32, TSharedPtr<FItem>> Items;
};
