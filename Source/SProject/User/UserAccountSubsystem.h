
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Templates/SharedPointer.h"
#include "UObject/ScriptInterface.h"

#include "UserAccountSubsystem.generated.h"

class IUserDocumentMiddleware;
struct FFetchDocument;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserConnectedEvent, int32, ErrCode);

/** 사용자 계정 관리 시스템*/
UCLASS()
class SPROJECT_API UUserAccountSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	static UUserAccountSubsystem* Get(const ULocalPlayer* InLocalPlayer);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void BindUserDocumentMiddleware(TScriptInterface<IUserDocumentMiddleware> Middleware);

	void UnbindUserDocumentMiddleware(TScriptInterface<IUserDocumentMiddleware> Middleware);

	UFUNCTION(BlueprintCallable, Category = "사용자 계정", meta = (ReturnDisplayName="Success"))
	bool Connect(FOnUserConnectedEvent ConnectEvent);

private:
	void OnFetchDocument();

private:
	UPROPERTY()
	TArray<TScriptInterface<IUserDocumentMiddleware>> UserDocumentMiddlewares;

	// 캐시 메모리
	TSharedPtr<FFetchDocument> FetchDocument;
};
