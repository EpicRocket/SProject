
#include "InventorySubsystem.h"
// include Engine
#include "Engine/LocalPlayer.h"
// include Project
#include "Types/FetchDocumentTypes.h"
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

void UInventorySubsystem::ApplyUserDocumentChanges(const TSharedRef<FFetchDocument> FetchDocument)
{
    if (FetchDocument->Gold.IsSet())
    {
        Gold = FetchDocument->Gold.GetValue();
    }

    if (FetchDocument->Cash.IsSet())
    {
        Cash = FetchDocument->Cash.GetValue();
    }

    if (FetchDocument->Items.IsSet())
    {
        for(auto const& FetchItem : FetchDocument->Items.GetValue())
		{
            GetItem(FetchItem.Key).Get() = FetchItem;
		}
    }
}

int64 UInventorySubsystem::GetGold() const
{
    return Gold;
}

int32 UInventorySubsystem::GetCash() const
{
    return Cash;
}

FItem UInventorySubsystem::Get(int32 Key) const
{
    return GetItem(Key).Get();
}

TSharedRef<FItem> UInventorySubsystem::GetItem(int32 Key) const
{
    if (!Items.Contains(Key))
    {
        auto NewItem = MakeShared<FItem>();
        NewItem->Key = Key;
        Items.Emplace(Key, NewItem);
    }
    return Items[Key].ToSharedRef();
}
