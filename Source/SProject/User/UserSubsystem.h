
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Interface/UserDocumentMiddleware.h"

#include "UserSubsystem.generated.h"


struct FFetchDocument;
struct FUserInfo;
class ULocalPlayer;

/** 유저 정보 관리 시스템 */
UCLASS()
class MY_API UUserSubsystem : public ULocalPlayerSubsystem, public IUserDocumentMiddleware
{
    GENERATED_BODY()
    
public:
    static UUserSubsystem* Get(const ULocalPlayer* InLocalPlayer);

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // IUserDocumentMiddleware
    virtual void ApplyUserDocumentChanges(const TSharedRef<FFetchDocument> FetchDocument) override;
    // ~IUserDocumentMiddleware

    UFUNCTION(BlueprintCallable, Category = "유저")
    FString GetUserName() const;

    UFUNCTION(BlueprintCallable, Category = "유저")
    FString GetUserTag() const;

    UFUNCTION(BlueprintCallable, Category = "유저")
    int32 GetLevel() const;

private:
    TSharedRef<FUserInfo> GetUserInfo() const;

private:
    mutable TSharedPtr<FUserInfo> UserInfo;
};
