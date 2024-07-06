
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"

#include "UserAccountSubsystem.generated.h"

class IUserDocumentMiddleware;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserConnectedEvent, int32, ErrCode);

/** 사용자 계정 관리 시스템*/
UCLASS()
class SPROJECT_API UUserAccountSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:
    static UUserAccountSubsystem* Get(const ULocalPlayer* InLocalPlayer);

    void BindUserDocumentMiddleware(IUserDocumentMiddleware* Middleware);

    void UnbindUserDocumentMiddleware(IUserDocumentMiddleware* Middleware);

    UFUNCTION(BlueprintCallable, Category = "사용자 계정", meta = (ReturnDisplayName="Success"))
    bool Connect(FOnUserConnectedEvent ConnectEvent);

private:
    UPROPERTY(Transient)
    TArray<IUserDocumentMiddleware*> UserDocumentMiddlewares;
};
