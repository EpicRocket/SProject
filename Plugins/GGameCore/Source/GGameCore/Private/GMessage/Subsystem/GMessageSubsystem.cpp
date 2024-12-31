
#include "GMessage/Subsystem/GMessageSubsystem.h"
// include Engine
#include "Engine/Engine.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GMessageSubsystem)

DEFINE_LOG_CATEGORY(LogGMessage);

//////////////////////////////////////////////////////////////////////////
// FGMessageListenerHandle
//////////////////////////////////////////////////////////////////////////

void FGMessageListenerHandle::Unregister()
{
	auto GMessageSubsystem = Subsystem.Get();
	if (!GMessageSubsystem)
	{
		return;
	}

	GMessageSubsystem->UnregisterListener(*this);
	Subsystem.Reset();
	Channel = FGameplayTag{};
	ID = 0;
}

//////////////////////////////////////////////////////////////////////////
// UGMessageSubsystem
//////////////////////////////////////////////////////////////////////////

void UGMessageSubsystem::Deinitialize()
{
	Listeners.Reset();
}

void UGMessageSubsystem::UnregisterListener(FGMessageListenerHandle Handle)
{
	if (Handle.IsValid())
	{
		check(Handle.Subsystem == this);
		UnregisterListenerInternal(Handle.Channel, Handle.ID);
	}
	else
	{
		UE_LOG(LogGMessage, Warning, TEXT("��ȿ���� ���� �ڵ� ����� ����Ϸ��� �մϴ�."));
	}
}

void UGMessageSubsystem::K2_BroadcastMessage(FGameplayTag Channel, const int32& Message)
{
	checkNoEntry();
}

DEFINE_FUNCTION(UGMessageSubsystem::execK2_BroadcastMessage)
{
	P_GET_STRUCT(FGameplayTag, Channel);

	Stack.MostRecentPropertyAddress = nullptr;
	Stack.StepCompiledIn<FStructProperty>(nullptr);
	void* MessagePtr = Stack.MostRecentPropertyAddress;
	FStructProperty* StructProp = CastField<FStructProperty>(Stack.MostRecentProperty);

	P_FINISH;

	if (ensure((StructProp != nullptr) && (StructProp->Struct != nullptr) && (MessagePtr != nullptr)))
	{
		P_THIS->BroadcastMessageInternal(Channel, StructProp->Struct, MessagePtr);
	}
}

void UGMessageSubsystem::BroadcastMessageInternal(FGameplayTag Channel, const UScriptStruct* StructType, const void* MessageBytes)
{
	bool bOnInitialTag = true;
	for (FGameplayTag Tag = Channel; Tag.IsValid(); Tag = Tag.RequestDirectParent())
	{
		auto Iter = Listeners.Find(Tag);
		if (!Iter)
		{
			continue;
		}

		TArray<FGMessageListenerData> Temp{ Iter->Listeners };
		for (const auto& Listener : Temp)
		{
			if (bOnInitialTag || !Listener.bMatchExact)
			{
				if (Listener.bHadValidType && !Listener.ListenerStructType.IsValid())
				{
					UE_LOG(LogGMessage, Warning, TEXT("ä�� %s���� ������ ����ü Ÿ���� ��ȿ���� �ʽ��ϴ�. ��Ͽ��� ������ ���� �մϴ�."), *Channel.ToString());
					UnregisterListenerInternal(Channel, Listener.HandleID);
					continue;
				}

				if (!Listener.bHadValidType || StructType->IsChildOf(Listener.ListenerStructType.Get()))
				{
					Listener.ReceivedCallback(Channel, StructType, MessageBytes);
				}
				else
				{
					UE_LOG(LogGMessage, Error, TEXT("ä�� %s���� ���� Ÿ�� ����ġ �մϴ�. (��ε�ĳ��Ʈ Ÿ�� %s�̰� %s�� �����ʰ� %s Ÿ���� ������)"),
						*Channel.ToString(),
						*StructType->GetPathName(),
						*Tag.ToString(),
						*Listener.ListenerStructType->GetPathName());
				}
			}
		}
		bOnInitialTag = false;
	}
}

FGMessageListenerHandle UGMessageSubsystem::RegisterListenerInternal(FGameplayTag Channel, TFunction<void(FGameplayTag, const UScriptStruct*, const void*)>&& Callback, const UScriptStruct* StructType, bool bMatchExact)
{
	auto& List = Listeners.FindOrAdd(Channel);

	auto& Entry = List.Listeners.AddDefaulted_GetRef();
	Entry.ReceivedCallback = MoveTemp(Callback);
	Entry.HandleID = ++List.HandleID;
	Entry.bMatchExact = bMatchExact;
	Entry.ListenerStructType = StructType;
	Entry.bHadValidType = StructType != nullptr;

	return FGMessageListenerHandle(this, Channel, Entry.HandleID);
}

void UGMessageSubsystem::UnregisterListenerInternal(FGameplayTag Channel, int32 HandleID)
{
	auto Iter = Listeners.Find(Channel);
	if (!Iter)
	{
		return;
	}

	int32 MatchIndex = Iter->Listeners.IndexOfByPredicate([ID = HandleID](const FGMessageListenerData& Other) {
		return Other.HandleID == ID;
	});

	if (MatchIndex != INDEX_NONE)
	{
		Iter->Listeners.RemoveAtSwap(MatchIndex);
	}

	if (Iter->Listeners.IsEmpty())
	{
		Listeners.Remove(Channel);
	}
}
