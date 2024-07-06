
#include "UserSubsystem.h"
// include Engine
#include "Engine/LocalPlayer.h"
// include Project
#include "Types/FetchDocument.h"
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
