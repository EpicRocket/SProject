
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Interface/UserDocumentMiddleware.h"

#include "UserSubsystem.generated.h"


struct FFetchDocument;
struct FUserInfo;
class ULocalPlayer;

/** 유저 정보 관리 시스템 */
UCLASS()
class SPROJECT_API UUserSubsystem : public ULocalPlayerSubsystem, public IUserDocumentMiddleware
{
    GENERATED_BODY()
    
public:
    static UUserSubsystem* Get(const ULocalPlayer* InLocalPlayer);

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    // IUserDocumentMiddleware
    virtual void ApplyUserDocumentChanges(const TSharedPtr<FFetchDocument> FetchDocument) override { /*구현 해야함*/ }
    virtual void FinalizeUserDocumentUpdate(const TSharedPtr<FFetchDocument> FetchDocument) override { /*구현 해야함*/ }
    // ~IUserDocumentMiddleware

private:
    TSharedPtr<FUserInfo> UserInfo;
};
