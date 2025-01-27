
#pragma once

#include "GameplayPlayer.h"

#include "GameplayUserPlayer.generated.h"

class AMyPlayerController;

UCLASS(BlueprintType)
class MY_API UGameplayUserPlayer : public UGameplayPlayer
{
	GENERATED_BODY()

public:
	virtual int64 GetItemCount(int32 Key) const override;

	virtual FGErrorInfo ConsumeItem(int32 Key, int32 Count) override;

public:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AMyPlayerController> OwningPlayerController;

};
