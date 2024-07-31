
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Templates/SharedPointer.h"
#include "UObject/ScriptInterface.h"

#include "UserAccountSubsystem.generated.h"

enum class EUserLoginType : uint8;
struct FUserAccount;
class IUserDocumentMiddleware;
struct FFetchDocument;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserLoginEvent, int32, ErrCode);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserLogoutEvent, int32, ErrCode);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnUserConnectedEvent, int32, ErrCode);

/** 사용자 계정 관리 시스템*/
UCLASS()
class MY_API UUserAccountSubsystem : public ULocalPlayerSubsystem
{
	GENERATED_BODY()

public:
	static UUserAccountSubsystem* Get(const ULocalPlayer* InLocalPlayer);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	void BindUserDocumentMiddleware(TScriptInterface<IUserDocumentMiddleware> Middleware);

	void UnbindUserDocumentMiddleware(TScriptInterface<IUserDocumentMiddleware> Middleware);

	UFUNCTION(BlueprintCallable, Category = "사용자 계정", meta = (ReturnDisplayName="Success"))
	bool Login(EUserLoginType LoginType, FOnUserLoginEvent LoginEvent);

	UFUNCTION(BlueprintCallable, Category = "사용자 계정", meta = (ReturnDisplayName = "Success"))
	bool ReLogin(FOnUserLoginEvent LoginEvent);

	UFUNCTION(BlueprintCallable, Category = "사용자 계정", meta = (ReturnDisplayName = "Success"))
	bool Logout(FOnUserLogoutEvent LogoutEvent);

	UFUNCTION(BlueprintCallable, Category = "사용자 계정", meta = (ReturnDisplayName="Success"))
	bool Connect(FOnUserConnectedEvent ConnectEvent);

	UFUNCTION(BlueprintCallable, Category = "사용자 계정")
	bool IsLogin() const;

private:
	TSharedRef<FUserAccount> GetUserAccount() const;

	void OnFetchDocument(TSharedPtr<FFetchDocument> FetchDocument);

private:
	EUserLoginType LoginType = static_cast<EUserLoginType>(0);

	mutable TSharedPtr<FUserAccount> UserAccount;

	UPROPERTY(Transient)
	TArray<TScriptInterface<IUserDocumentMiddleware>> UserDocumentMiddlewares;
};
