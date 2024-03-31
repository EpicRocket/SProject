

#include "MyGameplayEffectContext.h"

#include "AbilitySystem/MyAbilitySourceInterface.h"
#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyGameplayEffectContext)

class FArchive;

FMyGameplayEffectContext* FMyGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FMyGameplayEffectContext::StaticStruct()))
	{
		return (FMyGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FMyGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

void FMyGameplayEffectContext::SetAbilitySource(const IMyAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const IMyAbilitySourceInterface* FMyGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IMyAbilitySourceInterface>(AbilitySourceObject.Get());
}

const UPhysicalMaterial* FMyGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}

