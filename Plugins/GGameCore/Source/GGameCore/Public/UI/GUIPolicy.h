
#pragma once

#include "Engine/World.h"

#include "GUIPolicy.generated.h"

class UGLocalPlayer;
class AGPlayerController;
class UPrimaryGameLayout;

USTRUCT()
struct FRootViewportLayoutInfo
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TObjectPtr<UGLocalPlayer> LocalPlayer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UPrimaryGameLayout> RootLayout = nullptr;

	UPROPERTY(Transient)
	bool bAddedToViewport = false;

	bool operator==(const UGLocalPlayer* Other) const {return LocalPlayer == Other;}
};

UCLASS(Abstract, Blueprintable, Within = GUIManagerSubsystem)
class GGAMECORE_API UGUIPolicy : public UObject
{
	GENERATED_BODY()

public:
	void NotifyPlayerAdded(UGLocalPlayer* LocalPlayer);
	void NotifyPlayerRemoved(UGLocalPlayer* LocalPlayer);
	void NotifyPlayerDestroyed(UGLocalPlayer* LocalPlayer);

protected:
	void AddLayoutToViewport(UGLocalPlayer* LocalPlayer, UPrimaryGameLayout* Layout);

	void CreateLayoutWidget(UGLocalPlayer* LocalPlayer);

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	TSoftClassPtr<UPrimaryGameLayout> LayoutClass;

	UPROPERTY(Transient)
	TArray<FRootViewportLayoutInfo> RootViewportLayouts;
};
