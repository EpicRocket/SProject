
#pragma once

#include "CoreMinimal.h"

#include "MyUIPolicy.generated.h"

class ULocalPlayer;
class UMyGameLayout;

USTRUCT()
struct FMyRootViewportLayoutInfo
{
	GENERATED_BODY()

    UPROPERTY(Transient)
	TObjectPtr<ULocalPlayer> LocalPlayer = nullptr;

	UPROPERTY(Transient)
	TObjectPtr<UMyGameLayout> RootLayout = nullptr;

    UPROPERTY(Transient)
	bool bAddedToViewport = false;

	bool operator==(const ULocalPlayer* OtherLocalPlayer) const { return LocalPlayer == OtherLocalPlayer; }
};

UCLASS(Abstract, meta = (DisableNativeTick))
class SPROJECT_API UMyUIPolicy : public UObject
{
    GENERATED_BODY()

};
