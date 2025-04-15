
#include "UserAccountSubsystem.h"
// include Engine
#include "Engine/GameInstance.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/KismetSystemLibrary.h"
// include Project
#include "Types/GameErrCodeTypes.h"
#include "Types/FetchDocumentTypes.h"
#include "UserDocumentMiddleware.h"
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
	}
}

bool UUserAccountSubsystem::Login(EUserLoginType Type, FOnUserLoginEvent LoginEvent)
{
	auto LocalPlayer = GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		return false;
	}

	if (IsLogin())
	{
		return false;
	}

	LoginType = Type;

	switch (LoginType)
	{
	case EUserLoginType::Singleplay:
	{
		GetUserAccount()->ID = UKismetSystemLibrary::GetDeviceId();
		GetUserAccount()->Token = USingleplaySaveGameContext::Token;

		auto GameInstance = LocalPlayer->GetGameInstance();
		if (!IsValid(GameInstance))
		{
			return false;
		}

		auto SingleplaySubsystem = GameInstance->GetSubsystem<USingleplaySubsystem>();
		if (!IsValid(SingleplaySubsystem))
		{
			return false;
		}

		if (SingleplaySubsystem->GetSaveGame() == nullptr)
		{
			return false;
		}

		SingleplaySubsystem->Load();
		LoginEvent.ExecuteIfBound(static_cast<int32>(EGameErrCode::None));
	}
	break;

	case EUserLoginType::Guest:
	{
		// TODO: 寃뚯뒪??援ы쁽 ?꾩슂??
		LoginType = EUserLoginType::None;
		return false;
	}
	break;

	default:
	{
		LoginType = EUserLoginType::None;
		return false;
	}
	}

	return true;
}

bool UUserAccountSubsystem::ReLogin(FOnUserLoginEvent LoginEvent)
{
	switch (LoginType)
	{
	case EUserLoginType::Singleplay:
	{
		LoginEvent.ExecuteIfBound(static_cast<int32>(EGameErrCode::None));
	}
	break;

	case EUserLoginType::Guest:
	{
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

bool UUserAccountSubsystem::Logout(FOnUserLogoutEvent LogoutEvent)
{
	if (!IsLogin())
	{
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

bool UUserAccountSubsystem::Connect(FOnUserConnectedEvent ConnectEvent)
{
	if (!IsLogin())
	{
		return false;
	}

	switch (LoginType)
	{
	case EUserLoginType::Singleplay:
	{
		auto SingleplaySubsystem = USingleplaySubsystem::Get(GetLocalPlayer());
		if (!IsValid(SingleplaySubsystem))
		{
			return false;
		}

		auto Context = SingleplaySubsystem->GetSaveGame();
		if (!IsValid(Context))
		{
			return false;
		}

		auto FetchDocument = MakeShared<FFetchDocument>(Context->FetchDocument());
		OnFetchDocument(FetchDocument);
		ConnectEvent.ExecuteIfBound(static_cast<int32>(EGameErrCode::None));
	}
	break;

	case EUserLoginType::Guest:
	{
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

bool UUserAccountSubsystem::IsLogin() const
{
	switch (LoginType)
	{
	case EUserLoginType::Singleplay:
	{
	}
	break;

	case EUserLoginType::Guest:
	{
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

EUserLoginType UUserAccountSubsystem::GetUserLoginType() const
{
	return LoginType;
}

FString UUserAccountSubsystem::GetUserLoginTypeToString() const
{
	return FString();
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
		}
	}

	// Finalize FetchDocument
	for (auto& Middleware : UserDocumentMiddlewares)
	{
		Middleware->FinalizeUserDocumentUpdate(FetchDocument.ToSharedRef());
	}
}
