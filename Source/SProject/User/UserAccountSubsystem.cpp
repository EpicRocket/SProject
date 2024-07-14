
#include "UserAccountSubsystem.h"
// include Engine
#include "Engine/LocalPlayer.h"
#include "Kismet/KismetSystemLibrary.h"
// include Project
#include "Types/GameErrCodeTypes.h"
#include "Types/FetchDocumentTypes.h"
#include "Interface/UserDocumentMiddleware.h"
#include "Single/SingleplaySubsystem.h"
#include "Single/SingleplaySaveGame.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UserAccountSubsystem)


UUserAccountSubsystem* UUserAccountSubsystem::Get(const ULocalPlayer* InLocalPlayer)
{
	if (!IsValid(InLocalPlayer))
	{
		return nullptr;
	}
	return InLocalPlayer->GetSubsystem<UUserAccountSubsystem>();
}

void UUserAccountSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UUserAccountSubsystem::BindUserDocumentMiddleware(TScriptInterface<IUserDocumentMiddleware> Middleware)
{
	if (Middleware)
	{
		UserDocumentMiddlewares.Add(Middleware);
	}
	else
	{
		UE_LOG(LogCore, Warning, TEXT("UserDocumentMiddleware 인터페이스가 유효하지 않습니다."));
	}
}

void UUserAccountSubsystem::UnbindUserDocumentMiddleware(TScriptInterface<IUserDocumentMiddleware> Middleware)
{
	if (Middleware)
	{
		UserDocumentMiddlewares.Remove(Middleware);
	}
	else
	{
		UE_LOG(LogCore, Warning, TEXT("UserDocumentMiddleware 인터페이스가 유효하지 않습니다."));
	}
}

bool UUserAccountSubsystem::Login(EUserLoginType Type, FOnUserLoginEvent LoginEvent)
{
	if (IsLogin())
	{
		UE_LOG(LogCore, Error, TEXT("이미 로그인 상태입니다."));
		return false;
	}

	LoginType = Type;

	switch (LoginType)
	{
	case EUserLoginType::Singleplay:
	{
		GetUserAccount()->ID = UKismetSystemLibrary::GetDeviceId();
		GetUserAccount()->Token = USingleplaySaveGameContext::Token;

		LoginEvent.ExecuteIfBound(static_cast<int32>(EGameErrCode::None));
	}
	break;

	case EUserLoginType::Guest:
	{
		// TODO: 게스트 구현 필요함.
		LoginType = EUserLoginType::None;
		UE_LOG(LogCore, Warning, TEXT("게스트 로그인은 아직 구현되지 않았음."));
		return false;
	}
	break;

	default:
	{
		LoginType = EUserLoginType::None;
		UE_LOG(LogCore, Error, TEXT("로그인 타입이 유효하지 않습니다. [Type: %s]"), *UEnum::GetValueAsName(Type).ToString());
		return false;
	}
	}

	return true;
}

bool UUserAccountSubsystem::ReLogin(FOnUserLoginEvent LoginEvent)
{
	// TODO: 연결이 끊겼을 때 재로그인 구현이 필요함

	switch (LoginType)
	{
	case EUserLoginType::Singleplay:
	{
		// NOTE. 싱글플레이는 재연결 개념이 없다.
		LoginEvent.ExecuteIfBound(static_cast<int32>(EGameErrCode::None));
	}
	break;

	case EUserLoginType::Guest:
	{
		// TODO: 게스트 구현 필요함.
		UE_LOG(LogCore, Warning, TEXT("게스트 재로그인은 아직 구현되지 않았음."));
		return false;
	}
	break;

	default:
	{
		UE_LOG(LogCore, Error, TEXT("로그인 타입이 유효하지 않습니다. [Type: %s]"), *UEnum::GetValueAsName(LoginType).ToString());
		return false;
	}
	}

	return true;
}

bool UUserAccountSubsystem::Logout(FOnUserLogoutEvent LogoutEvent)
{
	if (!IsLogin())
	{
		UE_LOG(LogCore, Warning, TEXT("로그인 상태가 아닙니다."));
		return false;
	}

	switch (LoginType)
	{
	case EUserLoginType::Singleplay:
	{
		LoginType = EUserLoginType::None;
		GetUserAccount()->ID.Empty();
		GetUserAccount()->Token.Empty();
		LogoutEvent.ExecuteIfBound(static_cast<int32>(EGameErrCode::None));
	}
	break;

	case EUserLoginType::Guest:
	{
		// TODO: 게스트 구현 필요함.
		UE_LOG(LogCore, Warning, TEXT("게스트 로그아웃은 아직 구현되지 않았음."));
		return false;
	}
	break;

	default:
	{
		UE_LOG(LogCore, Error, TEXT("로그인 타입이 유효하지 않습니다. [Type: %s]"), *UEnum::GetValueAsName(LoginType).ToString());
		return false;
	}
	}

	return true;
}

bool UUserAccountSubsystem::Connect(FOnUserConnectedEvent ConnectEvent)
{
	if (!IsLogin())
	{
		UE_LOG(LogCore, Error, TEXT("로그인 상태가 아닙니다."));
		return false;
	}

	switch (LoginType)
	{
	case EUserLoginType::Singleplay:
	{
		auto SingleplaySubsystem = USingleplaySubsystem::Get(this);
		if (!IsValid(SingleplaySubsystem))
		{
			UE_LOG(LogCore, Error, TEXT("싱글플레이 서브시스템을 찾을 수 없습니다."));
			return false;
		}

		auto Context = SingleplaySubsystem->GetSaveGame();
		if (!IsValid(Context))
		{
			// NOTE: 싱글플레이 서브시스템을 사용하지 못하는 상태다.
			UE_LOG(LogCore, Error, TEXT("싱글플레이 정보를 찾을 수 없습니다."));
			return false;
		}

		auto FetchDocument = MakeShared<FFetchDocument>(Context->FetchDocument());
		OnFetchDocument(FetchDocument);
		ConnectEvent.ExecuteIfBound(static_cast<int32>(EGameErrCode::None));
	}
	break;

	case EUserLoginType::Guest:
	{
		// TODO: 게스트 구현 필요함.
		UE_LOG(LogCore, Warning, TEXT("게스트 연결은 아직 구현되지 않았음."));
		return false;
	}
	break;

	default:
	{
		UE_LOG(LogCore, Error, TEXT("로그인 타입이 유효하지 않습니다. [Type: %s]"), *UEnum::GetValueAsName(LoginType).ToString());
		return false;
	}
	}

	return true;
}

bool UUserAccountSubsystem::IsLogin() const
{
	switch (LoginType)
	{
	case EUserLoginType::Singleplay:
	{
		// NOTE: 싱글플레이는 별도 체크가 필요 없다.
	}
	break;

	case EUserLoginType::Guest:
	{
		// TODO: 미구현
		return false;
	}
	break;

	default:
	{
		return false;
	}
	}

	return true;
}

TSharedRef<FUserAccount> UUserAccountSubsystem::GetUserAccount() const
{
	if (!UserAccount.IsValid())
	{
		UserAccount = MakeShared<FUserAccount>();
	}
	return UserAccount.ToSharedRef();
}

void UUserAccountSubsystem::OnFetchDocument(TSharedPtr<FFetchDocument> FetchDocument)
{
	if (!FetchDocument.IsValid())
	{
		return;
	}

	// Apply FetchDocument
	for (auto& Middleware : UserDocumentMiddlewares)
	{
		if (Middleware)
		{
			Middleware->ApplyUserDocumentChanges(FetchDocument.ToSharedRef());
		}
		else
		{
			UE_LOG(LogCore, Warning, TEXT("UserDocumentMiddleware 인터페이스가 유효하지 않습니다."));
		}
	}

	// Finalize FetchDocument
	for (auto& Middleware : UserDocumentMiddlewares)
	{
		Middleware->FinalizeUserDocumentUpdate(FetchDocument.ToSharedRef());
	}
}
