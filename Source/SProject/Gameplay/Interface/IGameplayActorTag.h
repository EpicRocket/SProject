
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IGameplayActorTag.generated.h"

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class MY_API UGameplayActorTag : public UInterface
{
    GENERATED_BODY()
};

class MY_API IGameplayActorTag : public IInterface
{
    GENERATED_BODY()

	static const FString DotString;
    static const FString PositionTagName;

public:
    UFUNCTION(BlueprintCallable, Category = "Gameplay|Actor|Tag")
    virtual int32 GetPosition() const;
};
