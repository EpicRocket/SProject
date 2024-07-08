
#include "UserAccountSubsystem.h"
// include Engine
#include "Engine/LocalPlayer.h"
// include Project
#include "Types/GameErrCode.h"
#include "Types/FetchDocument.h"
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

	UserDocumentMiddlewares.Empty();
	FetchDocument = nullptr;
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

bool UUserAccountSubsystem::Connect(FOnUserConnectedEvent ConnectEvent)
{
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	if (!IsValid(LocalPlayer))
	{
		UE_LOG(LogCore, Error, TEXT("로컬 플레이어를 찾을 수 없습니다."));
		return false;
	}

	FetchDocument = nullptr;

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

	FetchDocument =  MakeShared<FFetchDocument>(Context->FetchDocument());

	if (!FetchDocument.IsValid())
	{
		UE_LOG(LogCore, Error, TEXT("사용자 정보를 찾을 수 없습니다."));
		return false;
	}

	OnFetchDocument();

	ConnectEvent.ExecuteIfBound(static_cast<int32>(EGameErrCode::None));
	return true;
}

void UUserAccountSubsystem::OnFetchDocument()
{
	if (!FetchDocument.IsValid())
	{
		return;
	}

	// Apply FetchDocument
	for (auto Middleware : UserDocumentMiddlewares)
	{
		if (Middleware)
		{
			Middleware->ApplyUserDocumentChanges(FetchDocument);
		}
		else
		{
			UE_LOG(LogCore, Warning, TEXT("UserDocumentMiddleware 인터페이스가 유효하지 않습니다."));
		}
	}

	// Finalize FetchDocument
	for (auto Middleware : UserDocumentMiddlewares)
	{
		Middleware->FinalizeUserDocumentUpdate(FetchDocument);
	}
}
