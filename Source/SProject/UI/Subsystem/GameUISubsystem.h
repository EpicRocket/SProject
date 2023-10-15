
#pragma once

#include "CoreMinimal.h"
#include "Containers/Ticker.h"
#include "GameUIManagerSubsystem.h"
#include "GameUISubsystem.generated.h"

UCLASS(Config = SProject)
class UGameUISubsystem : public UGameUIManagerSubsystem
{
    GENERATED_BODY()

public:
    UGameUISubsystem();

    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

private:
    bool Tick(float DeltaTime);
	void SyncRootLayoutVisibilityToShowHUD();
	
private:
	FTSTicker::FDelegateHandle TickHandle;
};
