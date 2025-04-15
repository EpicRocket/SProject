
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
		UE_LOG(LogCore, Warning, TEXT("UserDocumentMiddleware ?명꽣?섏씠?ㅺ? ?좏슚?섏? ?딆뒿?덈떎."));
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
		UE_LOG(LogCore, Warning, TEXT("UserDocumentMiddleware ?명꽣?섏씠?ㅺ? ?좏슚?섏? ?딆뒿?덈떎."));
	}
}

bool UUserAccountSubsystem::Login(EUserLoginType Type, FOnUserLoginEvent LoginEvent)
{
	auto LocalPlayer = GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		UE_LOG(LogCore, Error, TEXT("濡쒖뺄 ?뚮젅?댁뼱瑜?李얠쓣 ???놁뒿?덈떎."));
		return false;
	}

	if (IsLogin())
	{
		UE_LOG(LogCore, Error, TEXT("?대? 濡쒓렇???곹깭?낅땲??"));
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
			UE_LOG(LogCore, Error, TEXT("寃뚯엫 ?몄뒪?댁뒪瑜?李얠쓣 ???놁뒿?덈떎."));
			return false;
		}

		auto SingleplaySubsystem = GameInstance->GetSubsystem<USingleplaySubsystem>();
		if (!IsValid(SingleplaySubsystem))
		{
			UE_LOG(LogCore, Error, TEXT("?깃??뚮젅???쒕툕?쒖뒪?쒖쓣 李얠쓣 ???놁뒿?덈떎."));
			return false;
		}

		if (SingleplaySubsystem->GetSaveGame() == nullptr)
		{
			UE_LOG(LogCore, Error, TEXT("?깃??뚮젅???뺣낫瑜?李얠쓣 ???놁뒿?덈떎."));
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
		UE_LOG(LogCore, Warning, TEXT("寃뚯뒪??濡쒓렇?몄? ?꾩쭅 援ы쁽?섏? ?딆븯??"));
		return false;
	}
	break;

	default:
	{
		LoginType = EUserLoginType::None;
		UE_LOG(LogCore, Error, TEXT("濡쒓렇????낆씠 ?좏슚?섏? ?딆뒿?덈떎. [Type: %s]"), *UEnum::GetValueAsName(Type).ToString());
		return false;
	}
	}

	return true;
}

bool UUserAccountSubsystem::ReLogin(FOnUserLoginEvent LoginEvent)
{
	// TODO: ?곌껐???딄꼈?????щ줈洹몄씤 援ы쁽???꾩슂??

	switch (LoginType)
	{
	case EUserLoginType::Singleplay:
	{
		// NOTE. ?깃??뚮젅?대뒗 ?ъ뿰寃?媛쒕뀗???녿떎.
		LoginEvent.ExecuteIfBound(static_cast<int32>(EGameErrCode::None));
	}
	break;

	case EUserLoginType::Guest:
	{
		// TODO: 寃뚯뒪??援ы쁽 ?꾩슂??
		UE_LOG(LogCore, Warning, TEXT("寃뚯뒪???щ줈洹몄씤? ?꾩쭅 援ы쁽?섏? ?딆븯??"));
		return false;
	}
	break;

	default:
	{
		UE_LOG(LogCore, Error, TEXT("濡쒓렇????낆씠 ?좏슚?섏? ?딆뒿?덈떎. [Type: %s]"), *UEnum::GetValueAsName(LoginType).ToString());
		return false;
	}
	}

	return true;
}

bool UUserAccountSubsystem::Logout(FOnUserLogoutEvent LogoutEvent)
{
	if (!IsLogin())
	{
		UE_LOG(LogCore, Warning, TEXT("濡쒓렇???곹깭媛 ?꾨떃?덈떎."));
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
		// TODO: 寃뚯뒪??援ы쁽 ?꾩슂??
		UE_LOG(LogCore, Warning, TEXT("寃뚯뒪??濡쒓렇?꾩썐? ?꾩쭅 援ы쁽?섏? ?딆븯??"));
		return false;
	}
	break;

	default:
	{
		UE_LOG(LogCore, Error, TEXT("濡쒓렇????낆씠 ?좏슚?섏? ?딆뒿?덈떎. [Type: %s]"), *UEnum::GetValueAsName(LoginType).ToString());
		return false;
	}
	}

	return true;
}

bool UUserAccountSubsystem::Connect(FOnUserConnectedEvent ConnectEvent)
{
	if (!IsLogin())
	{
		UE_LOG(LogCore, Error, TEXT("濡쒓렇???곹깭媛 ?꾨떃?덈떎."));
		return false;
	}

	switch (LoginType)
	{
	case EUserLoginType::Singleplay:
	{
		auto SingleplaySubsystem = USingleplaySubsystem::Get(GetLocalPlayer());
		if (!IsValid(SingleplaySubsystem))
		{
			UE_LOG(LogCore, Error, TEXT("?깃??뚮젅???쒕툕?쒖뒪?쒖쓣 李얠쓣 ???놁뒿?덈떎."));
			return false;
		}

		auto Context = SingleplaySubsystem->GetSaveGame();
		if (!IsValid(Context))
		{
			// NOTE: ?깃??뚮젅???쒕툕?쒖뒪?쒖쓣 ?ъ슜?섏? 紐삵븯???곹깭??
			UE_LOG(LogCore, Error, TEXT("?깃??뚮젅???뺣낫瑜?李얠쓣 ???놁뒿?덈떎."));
			return false;
		}

		auto FetchDocument = MakeShared<FFetchDocument>(Context->FetchDocument());
		OnFetchDocument(FetchDocument);
		ConnectEvent.ExecuteIfBound(static_cast<int32>(EGameErrCode::None));
	}
	break;

	case EUserLoginType::Guest:
	{
		// TODO: 寃뚯뒪??援ы쁽 ?꾩슂??
		UE_LOG(LogCore, Warning, TEXT("寃뚯뒪???곌껐? ?꾩쭅 援ы쁽?섏? ?딆븯??"));
		return false;
	}
	break;

	default:
	{
		UE_LOG(LogCore, Error, TEXT("濡쒓렇????낆씠 ?좏슚?섏? ?딆뒿?덈떎. [Type: %s]"), *UEnum::GetValueAsName(LoginType).ToString());
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
		// NOTE: ?깃??뚮젅?대뒗 蹂꾨룄 泥댄겕媛 ?꾩슂 ?녿떎.
	}
	break;

	case EUserLoginType::Guest:
	{
		// TODO: 誘멸뎄??
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
			UE_LOG(LogCore, Warning, TEXT("UserDocumentMiddleware ?명꽣?섏씠?ㅺ? ?좏슚?섏? ?딆뒿?덈떎."));
		}
	}

	// Finalize FetchDocument
	for (auto& Middleware : UserDocumentMiddlewares)
	{
		Middleware->FinalizeUserDocumentUpdate(FetchDocument.ToSharedRef());
	}
}
