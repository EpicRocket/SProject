
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GMessage/GMessageTypes.h"

#include "GMessageSubsystem.generated.h"

USTRUCT(BlueprintType)
struct GGAMECORE_API FGMessageListenerHandle
{
    GENERATED_BODY()

    FGMessageListenerHandle() {}

    void Unregister();

    bool IsValid() const { return ID != 0; }

private:
    friend UGMessageSubsystem;

    UPROPERTY(Transient)
    TWeakObjectPtr<UGMessageSubsystem> Subsystem;

    UPROPERTY(Transient)
    FGameplayTag Channel;

    UPROPERTY(Transient)
    int32 ID = 0;

    FDelegateHandle StateClearedHandle;

    FGMessageListenerHandle(UGMessageSubsystem* InSubsystem, FGameplayTag InChannel, int32 InID) : Subsystem(InSubsystem), Channel(InChannel), ID(InID) {}
};

USTRUCT()
struct GGAMECORE_API FGMessageListenerData
{
    GENERATED_BODY()

    TFunction<void(FGameplayTag, const UScriptStruct*, const void*)> ReceivedCallback;

    int32 HandleID = INDEX_NONE;

    bool bMatchExact = false;

    TWeakObjectPtr<const UScriptStruct> ListenerStructType = nullptr;

    bool bHadValidType = false;
};

UCLASS()
class GGAMECORE_API UGMessageSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

    friend FGMessageListenerHandle;

public:
    virtual void Deinitialize() override;

    template <typename T>
    void BroadcastMessage(FGameplayTag Channel, const T& Message)
    {
        const UScriptStruct* StructType = TBaseStructure<T>::Get();
        BroadcastMessageInternal(Channel, StructType, &Message);
    }

    template <typename T>
    FGMessageListenerHandle RegisterListener(FGameplayTag Channel, TFunction<void(FGameplayTag, const T&)>&& Callback, bool bMatchExact = true)
    {
        auto ThunkCallback = [InnerCallback = MoveTemp(Callback)](FGameplayTag ActualTag, const UScriptStruct* SenderStructType, const void* SenderPayload)
            {
                InnerCallback(ActualTag, *reinterpret_cast<const T*>(SenderPayload));
            };

        const UScriptStruct* StructType = TBaseStructure<T>::Get();
        return RegisterListenerInternal(Channel, ThunkCallback, StructType, bMatchExact);
    }

    template <typename T, typename TOwner = UObject>
    FGMessageListenerHandle RegisterListener(FGameplayTag Channel, TOwner* Object, void(TOwner::* Function)(FGameplayTag, const T&))
    {
        TWeakObjectPtr<TOwner> WeakObject(Object);
        return RegisterListener<T>(Channel,
            [WeakObject, Function](FGameplayTag Channel, const T& Payload)
            {
                if (TOwner* StrongObject = WeakObject.Get())
                {
                    (StrongObject->*Function)(Channel, Payload);
                }
            });
    }

    template <typename T>
    FGMessageListenerHandle RegisterListener(FGameplayTag Channel, FGMessageListenerParams<T>& Params)
    {
        FGMessageListenerHandle Handle;

        // Register to receive any future messages broadcast on this channel
        if (Params.OnMessageReceivedCallback)
        {
            auto ThunkCallback = [InnerCallback = Params.OnMessageReceivedCallback](FGameplayTag ActualTag, const UScriptStruct* SenderStructType, const void* SenderPayload)
                {
                    InnerCallback(ActualTag, *reinterpret_cast<const T*>(SenderPayload));
                };

            const UScriptStruct* StructType = TBaseStructure<T>::Get();
            Handle = RegisterListenerInternal(Channel, ThunkCallback, StructType, Params.bMatchExact);
        }

        return Handle;
    }

    void UnregisterListener(FGMessageListenerHandle Handle);

protected:
    void BroadcastMessageInternal(FGameplayTag Channel, const UScriptStruct* StructType, const void* MessageBytes);

    FGMessageListenerHandle RegisterListenerInternal(
        FGameplayTag Channel,
        TFunction<void(FGameplayTag, const UScriptStruct*, const void*)>&& Callback,
        const UScriptStruct* StructType,
        bool bMatchExact);

    void UnregisterListenerInternal(FGameplayTag Channel, int32 HandleID);

private:
    struct FChannelListenerList
    {
        TArray<FGMessageListenerData> Listeners;
        int32 HandleID = 0;
    };

    TMap<FGameplayTag, FChannelListenerList> ListenerMap;
};
