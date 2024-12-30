
#pragma

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

template<typename T>
struct FGMessageListenerParams
{
    bool bMatchExact = true;

	TFunction<void(FGameplayTag, const T&)> OnMessageReceivedCallback;
    
    template<typename T = UObject>
	void SetMessageReceivedCallback(T* Object, void(T::* Function)(FGameplayTag, const T&))
	{
		TWeakObjectPtr<T> WeakObject(Object);
		OnMessageReceivedCallback = [WeakObject, Function](FGameplayTag Channel, const T& Payload)
		{
			if (T* StrongObject = WeakObject.Get())
			{
				(StrongObject->*Function)(Channel, Payload);
			}
		};
	}
};
