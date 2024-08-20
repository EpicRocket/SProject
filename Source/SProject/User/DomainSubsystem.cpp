
#include "DomainSubsystem.h"
// include Engine
#include "Engine/LocalPlayer.h"
// include Project
#include "Types/FetchDocumentTypes.h"
#include "UserAccountSubsystem.h"
#include "Table/TowerTable.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DomainSubsystem)


UDomainSubsystem* UDomainSubsystem::Get(const ULocalPlayer* InLocalPlayer)
{
    if (!IsValid(InLocalPlayer))
    {
        return nullptr;
    }
    return InLocalPlayer->GetSubsystem<UDomainSubsystem>();
}

void UDomainSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);

    if (auto Subsystem = Collection.InitializeDependency<UUserAccountSubsystem>())
    {
        Subsystem->BindUserDocumentMiddleware(this);
    }
}

void UDomainSubsystem::Deinitialize()
{
    if (auto Subsystem = UUserAccountSubsystem::Get(GetLocalPlayer()))
    {
        Subsystem->UnbindUserDocumentMiddleware(this);
    }

    Super::Deinitialize();
}

bool UDomainSubsystem::IsBuildableTower(const FTowerTableRow& TowerData) const
{
    // TODO: 구현 필요
    return true;
}
