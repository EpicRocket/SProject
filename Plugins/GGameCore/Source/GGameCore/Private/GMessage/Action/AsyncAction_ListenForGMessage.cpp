
#include "GMessage/Action/AsyncAction_ListenForGMessage.h"
// include Engine
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/GameInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AsyncAction_ListenForGMessage)

UAsyncAction_ListenForGMessage* UAsyncAction_ListenForGMessage::ListenForGMessages(UObject* WorldContextObject, FGameplayTag Channel, UScriptStruct* PayloadType, bool bMatchExact)
{
	auto World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		return nullptr;
	}

	auto Action = NewObject<UAsyncAction_ListenForGMessage>();
	Action->WorldPtr = World;
	Action->ChannelToRegister = Channel;
	Action->MessageStructType = PayloadType;
	Action->bMatchExact = bMatchExact;
	Action->RegisterWithGameInstance(World);

	return Action;
}

bool UAsyncAction_ListenForGMessage::GetPayload(UPARAM(ref)int32& OutPayload)
{
	checkNoEntry();
	return false;
}

DEFINE_FUNCTION(UAsyncAction_ListenForGMessage::execGetPayload)
{
	Stack.MostRecentPropertyAddress = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* MessagePtr = Stack.MostRecentPropertyAddress;
	FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);
	P_FINISH;

	bool bSuccess = false;

	if ((StructProp != nullptr) && (StructProp->Struct != nullptr) && (MessagePtr != nullptr) && (StructProp->Struct == P_THIS->MessageStructType.Get()) && (P_THIS->ReceivedMessagePayloadPtr != nullptr))
	{
		StructProp->Struct->CopyScriptStruct(MessagePtr, P_THIS->ReceivedMessagePayloadPtr);
		bSuccess = true;
	}

	*(bool*)RESULT_PARAM = bSuccess;
}

void UAsyncAction_ListenForGMessage::Activate()
{
	auto World = WorldPtr.Get();
	if (!World)
	{
		SetReadyToDestroy();
		return;
	}

	auto Router = UGameInstance::GetSubsystem<UGMessageSubsystem>(World->GetGameInstance());
	if (!Router)
	{
		SetReadyToDestroy();
		return;
	}

	TWeakObjectPtr<UAsyncAction_ListenForGMessage> WeakThis(this);
	ListenerHandle = Router->RegisterListenerInternal(ChannelToRegister,
		[WeakThis](FGameplayTag Channel, const UScriptStruct* StructType, const void* Payload)
		{
			if (UAsyncAction_ListenForGMessage* StrongThis = WeakThis.Get())
			{
				StrongThis->HandleMessageReceived(Channel, StructType, Payload);
			}
		},
		MessageStructType.Get(),
		bMatchExact);
}

void UAsyncAction_ListenForGMessage::SetReadyToDestroy()
{
	ListenerHandle.Unregister();

	Super::SetReadyToDestroy();
}

void UAsyncAction_ListenForGMessage::HandleMessageReceived(FGameplayTag Channel, const UScriptStruct* StructType, const void* Payload)
{
	if (!MessageStructType.Get() || (MessageStructType.Get() == StructType))
	{
		ReceivedMessagePayloadPtr = Payload;
		OnMessageReceived.Broadcast(this, Channel);
		ReceivedMessagePayloadPtr = nullptr;
	}

	if (!OnMessageReceived.IsBound())
	{
		SetReadyToDestroy();
	}
}
