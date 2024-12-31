
#pragma once

#include "CoreMinimal.h"
#include "Engine/CancellableAsyncAction.h"
#include "GameplayTagContainer.h"
#include "GMessage/Subsystem/GMessageSubsystem.h"

#include "AsyncAction_ListenForGMessage.generated.h"

class UScriptStruct;
class UWorld;
struct FFrame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAsyncGMessageDelegate, UAsyncAction_ListenForGMessage*, ProxyObject, FGameplayTag, ActualChannel);

UCLASS(BlueprintType, meta = (HasDedicatedAsyncNode))
class GGAMECORE_API UAsyncAction_ListenForGMessage : public UCancellableAsyncAction
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = Messaging, meta = (WorldContext = "WorldContextObject", BlueprintInternalUseOnly = "true"))
	static UAsyncAction_ListenForGMessage* ListenForGMessages(UObject* WorldContextObject, FGameplayTag Channel, UScriptStruct* PayloadType, bool bMatchExact = true);

	UFUNCTION(BlueprintCallable, CustomThunk, Category = "Messaging", meta = (CustomStructureParam = "OutPayload"))
	bool GetPayload(UPARAM(ref) int32& OutPayload);

	DECLARE_FUNCTION(execGetPayload);

	virtual void Activate() override;

	virtual void SetReadyToDestroy() override;

private:
	void HandleMessageReceived(FGameplayTag Channel, const UScriptStruct* StructType, const void* Payload);

public:
	UPROPERTY(BlueprintAssignable)
	FAsyncGMessageDelegate OnMessageReceived;

private:
	const void* ReceivedMessagePayloadPtr = nullptr;

	TWeakObjectPtr<UWorld> WorldPtr;
	FGameplayTag ChannelToRegister;
	TWeakObjectPtr<UScriptStruct> MessageStructType = nullptr;
	bool bMatchExact = true;

	FGMessageListenerHandle ListenerHandle;
};
