
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "UserDocumentMiddleware.h"

#include "UserSubsystem.generated.h"


struct FFetchDocument;
struct FUserInfo;
class ULocalPlayer;

/** ?좎? ?뺣낫 愿由??쒖뒪??*/
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

    UFUNCTION(BlueprintCallable, Category = "?좎?")
    FString GetUserName() const;

    UFUNCTION(BlueprintCallable, Category = "?좎?")
    FString GetUserTag() const;

    UFUNCTION(BlueprintCallable, Category = "?좎?")
    int32 GetLevel() const;

private:
    TSharedRef<FUserInfo> GetUserInfo() const;

private:
    mutable TSharedPtr<FUserInfo> UserInfo;
};
