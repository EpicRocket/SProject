
#pragma once

#include "Engine/DataAsset.h"
#include "NativeGameplayTags.h"

#include "GameplayUnitAnimState.generated.h"

namespace Unit
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Unit_AnimState_Attack);

	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Unit_AnimState_Death);
}

USTRUCT(BlueprintType)
struct MY_API FGameplayUnitAnimState
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag AnimState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<class UAnimMontage*> AnimMontages;

};

UCLASS()
class MY_API UGameplayUnitAnimStateDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ForceInlineRow, Categories = "Unit.AnimState"))
	TMap<FGameplayTag, FGameplayUnitAnimState> AnimStates;

};
