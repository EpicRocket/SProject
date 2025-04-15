
#include "UserSubsystem.h"
// include Engine
#include "Engine/LocalPlayer.h"
// include Project
#include "Types/FetchDocumentTypes.h"
#include "UserAccountSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UserSubsystem)


UUserSubsystem* UUserSubsystem::Get(const ULocalPlayer* InLocalPlayer)
{
	if (!IsValid(InLocalPlayer))
	{
		return nullptr;
	}
	return InLocalPlayer->GetSubsystem<UUserSubsystem>();
}

void UUserSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	if (auto Subsystem = Collection.InitializeDependency<UUserAccountSubsystem>())
	{
		Subsystem->BindUserDocumentMiddleware(this);
	}
}

void UUserSubsystem::Deinitialize()
{
	if (auto Subsystem = UUserAccountSubsystem::Get(GetLocalPlayer()))
	{
		Subsystem->UnbindUserDocumentMiddleware(this);
	}

	Super::Deinitialize();
}

void UUserSubsystem::ApplyUserDocumentChanges(const TSharedRef<FFetchDocument> FetchDocument)
{
	if (FetchDocument->UserInfo.IsSet())
	{
		GetUserInfo().Get() = FetchDocument->UserInfo.GetValue();
	}
	//FetchDocument->UserInfo.Get(GetUserInfo().Get());
}

FString UUserSubsystem::GetUserName() const
{
	return GetUserInfo()->Name;
}

FString UUserSubsystem::GetUserTag() const
{
	return GetUserInfo()->Tag;
}

int32 UUserSubsystem::GetLevel() const
{
	return GetUserInfo()->Level;
}

TSharedRef<FUserInfo> UUserSubsystem::GetUserInfo() const
{
	if (!UserInfo.IsValid())
	{
		UserInfo = MakeShared<FUserInfo>();
	}
	return UserInfo.ToSharedRef();
}
