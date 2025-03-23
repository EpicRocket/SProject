
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "NativeGameplayTags.h"

#include "IStageUnitAnimMontage.generated.h"

class UAnimMontage;

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class MY_API UStageUnitAnimMontage : public UInterface
{
    GENERATED_BODY()
};

class MY_API IStageUnitAnimMontage : public IInterface
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "Unit|AnimState")
    virtual UAnimMontage* GetAnimMontage(UPARAM(meta = (Categories = "AnimState")) FGameplayTag AnimState) const { return nullptr; }

};
