
#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/SoftObjectPtr.h"

#include "GUIManagerSubsystem.generated.h"

class UGLocalPlayer;
class UGUIPolicy;

UCLASS(Abstract, Config = Game)
class GGAMECORE_API UGUIManagerSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;
    virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    void NotifyPlayerAdded(UGLocalPlayer* LocalPlayer);
    void NotifyPlayerRemoved(UGLocalPlayer* LocalPlayer);
    void NotifyPlayerDestroyed(UGLocalPlayer* LocalPlayer);

    UFUNCTION(BlueprintCallable)
    UGUIPolicy* GetPolicy() const;

private:
    UPROPERTY(Transient)
    TObjectPtr<UGUIPolicy> GUIPolicy;

};
