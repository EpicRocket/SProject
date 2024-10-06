#include "AbilitySystem/GGamePlayEffectContext.h"

#include "AbilitySystem/GAbilitySourceInterface.h"

FGGameplayEffectContext* FGGameplayEffectContext::ExtractEffectContext(FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FGGameplayEffectContext::StaticStruct()))
	{
		return (FGGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

void FGGameplayEffectContext::SetAbilitySource(const IGAbilitySourceInterface* InObject, float InSourceLevel)
{
	AbilitySourceObject = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	//SourceLevel = InSourceLevel;
}

const IGAbilitySourceInterface* FGGameplayEffectContext::GetAbilitySource() const
{
	return Cast<IGAbilitySourceInterface>(AbilitySourceObject.Get());
}

bool FGGameplayEffectContext::NetSerialize(FArchive& Ar, UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	// Not serialized for post-activation use:
	// CartridgeID

	return true;
}

const UPhysicalMaterial* FGGameplayEffectContext::GetPhysicalMaterial() const
{
	if (const FHitResult* HitResultPtr = GetHitResult())
	{
		return HitResultPtr->PhysMaterial.Get();
	}
	return nullptr;
}
