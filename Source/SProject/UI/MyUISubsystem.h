
#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "UObject/SoftObjectPtr.h"

#include "MyUISubsystem.generated.h"

class UMyUIPolicy;
class UMyLocalPlayer;

UCLASS(Abstract, Config = Game)
class SPROJECT_API UMyUISubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

    UFUNCTION(BlueprintCallable, Category = "UI")
    UGameUIPolicy* GetCurrentUIPolicy() const { return Policy; }

	virtual void NotifyPlayerAdded(UMyLocalPlayer* LocalPlayer);
	virtual void NotifyPlayerRemoved(UMyLocalPlayer* LocalPlayer);
	virtual void NotifyPlayerDestroyed(UMyLocalPlayer* LocalPlayer);

private:
    UPROPERTY(Transient)
    TObjectPtr<UMyUIPolicy> Policy = nullptr;

    UPROPERTY(Config, EditAnywhere)
    TSoftClassPtr<UMyUIPolicy> PolicyClass;
};
