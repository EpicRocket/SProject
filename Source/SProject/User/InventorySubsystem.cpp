
#include "InventorySubsystem.h"
// include Engine
#include "Engine/LocalPlayer.h"
// include Project
#include "Types/FetchDocument.h"
#include "UserAccountSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventorySubsystem)


UInventorySubsystem* UInventorySubsystem::Get(const ULocalPlayer* InLocalPlayer)
{
    if (!IsValid(InLocalPlayer))
    {
        return nullptr;
    }
    return InLocalPlayer->GetSubsystem<UInventorySubsystem>();
}

void UInventorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (auto Subsystem = Collection.InitializeDependency<UUserAccountSubsystem>())
    {
        Subsystem->BindUserDocumentMiddleware(this);
    }
}

void UInventorySubsystem::Deinitialize()
{
    if (auto Subsystem = UUserAccountSubsystem::Get(GetLocalPlayer()))
    {
        Subsystem->UnbindUserDocumentMiddleware(this);
    }

    Super::Deinitialize();
}

int64 UInventorySubsystem::GetGold() const
{
    return Gold;
}

int32 UInventorySubsystem::GetCash() const
{
    return Cash;
}
