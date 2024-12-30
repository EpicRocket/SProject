
#include "GMessage/Subsystem/GMessageSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GMessageSubsystem)

//////////////////////////////////////////////////////////////////////////
// FGMessageListenerHandle
//////////////////////////////////////////////////////////////////////////

void FGMessageListenerHandle::Unregister()
{
}

//////////////////////////////////////////////////////////////////////////
// UGMessageSubsystem
//////////////////////////////////////////////////////////////////////////

void UGMessageSubsystem::Deinitialize()
{
}

void UGMessageSubsystem::UnregisterListener(FGMessageListenerHandle Handle)
{
}

void UGMessageSubsystem::BroadcastMessageInternal(FGameplayTag Channel, const UScriptStruct* StructType, const void* MessageBytes)
{
}

FGMessageListenerHandle UGMessageSubsystem::RegisterListenerInternal(FGameplayTag Channel, TFunction<void(FGameplayTag, const UScriptStruct*, const void*)>&& Callback, const UScriptStruct* StructType, bool bMatchExact)
{
	return FGMessageListenerHandle();
}

void UGMessageSubsystem::UnregisterListenerInternal(FGameplayTag Channel, int32 HandleID)
{
}
