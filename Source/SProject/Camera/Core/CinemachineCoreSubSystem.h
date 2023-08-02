

#pragma once

#include "Subsystems/WorldSubsystem.h"
#include "GameFramework/GameplayMessageTypes2.h"
#include "GameplayTagContainer.h"
#include "UObject/WeakObjectPtr.h"
#include "CinemachineBlend.h"
#include "Camera/Interface/CinemachineMixerInterface.h"
#include "CinemachineCoreSubSystem.generated.h"

class UCinemachineVirtualCameraBaseComponent;
class UCinemachineBrainComponent;
class USceneComponent;

/** UPROPERTY 속성을 적용하기 위한 Array 구조체 */
USTRUCT()
struct FCVCameraArray
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TArray<UCinemachineVirtualCameraBaseComponent*> VirtualCameras;
};

USTRUCT()
struct FCVBlendEventParams
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TScriptInterface<ICinemachineMixerInterface> Origin;

	UPROPERTY()
	UCinemachineBlend* Blend;
};

USTRUCT(BlueprintType)
struct FCinemachineGameplayMessageListenerHandle
{
	GENERATED_BODY()

	friend UCinemachineCoreSubSystem;

public:
	FCinemachineGameplayMessageListenerHandle() = default;

	void Unregister();

	bool IsValid() const { return ID != 0; }

private:
	FCinemachineGameplayMessageListenerHandle(UCinemachineCoreSubSystem* InSubsystem, FGameplayTag InChannel, int32 InID) : Subsystem(InSubsystem), Channel(InChannel), ID(InID) {}

private:
	UPROPERTY(Transient)
	TWeakObjectPtr<UCinemachineCoreSubSystem> Subsystem;

	UPROPERTY(Transient)
	FGameplayTag Channel;

	UPROPERTY(Transient)
	int32 ID = 0;

	FDelegateHandle StateClearedHandle;
};

USTRUCT()
struct FCinemachineGameplayMessageListenerData
{
	GENERATED_BODY()

public:
	// Callback for when a message has been received
	TFunction<void(FGameplayTag)> ReceivedCallback;

	int32 HandleID;
	EGameplayMessageMatch MatchType;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCinemachineCameraEvent, TScriptInterface<ICinemachineMixerInterface>, Mixer, TScriptInterface<ICinemachineCameraInterface>, ICamera);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCinemachineBrainEvent, UCinemachineBrainComponent*, BrainComponent);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCinemachineBlendEvent, FCVBlendEventParams, BlendParams);

/**
 * CineamchineBrainComponent 및 CinemachineVirtualCameraBaseComponent의 전체 목록과 가상 카메라의 우선 순위를 관리 합니다.
*/
UCLASS()
class UCinemachineCoreSubSystem : public UWorldSubsystem
{
	GENERATED_BODY()

	struct FChannelListenerList
	{
		TArray<FCinemachineGameplayMessageListenerData> Listeners;
		int32 HandleID = 0;
	};
public:
	// USubsystem implementation Begin
	virtual void Deinitialize() override;
	// USubsystem implementation End

	UFUNCTION(BlueprintPure, Category = "Cinemachine|ETC")
	double GetUnpausedTimeSeconds() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Camera")
	int32 GetActiveCameraCount() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Camera")
	UCinemachineVirtualCameraBaseComponent* GetActiveCamera(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Camera")
	void AddActiveCamera(UCinemachineVirtualCameraBaseComponent* VCamera);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Camera")
	void RemoveActiveCamera(UCinemachineVirtualCameraBaseComponent* VCamera);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Camera")
	void CameraEnabled(UCinemachineVirtualCameraBaseComponent* VCamera);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Camera")
	void CameraDisabled(UCinemachineVirtualCameraBaseComponent* VCamera);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Camera")
	void UpdateAllActiveVirtualCameras(FVector WorldUp, float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Camera")
	void UpdateVirtualCamera(UCinemachineVirtualCameraBaseComponent* VCamera, FVector WorldUp, float DeltaTime);

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Camera")
	USceneComponent* GetUpdateTarget(UCinemachineVirtualCameraBaseComponent* VCamera);

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Brain")
	int32 GetActiveBrainCount() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Brain")
	UCinemachineBrainComponent* GetActiveBrain(int32 Index) const;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Brain")
	void AddActiveBrain(UCinemachineBrainComponent* Brain);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Brain")
	void RemoveActiveBrain(UCinemachineBrainComponent* Brain);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsLive(TScriptInterface<ICinemachineCameraInterface> ICamera);

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsLiveInBlend(TScriptInterface<ICinemachineCameraInterface> ICamera);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void ResetCameraState();

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	UCinemachineBrainComponent* FindPotentialTargetBrain(UCinemachineVirtualCameraBaseComponent* VCamera);

	FCinemachineGameplayMessageListenerHandle RegisterListener(FGameplayTag Channel, TFunction<void(FGameplayTag)>&& Callback, EGameplayMessageMatch MatchType = EGameplayMessageMatch::ExactMatch);

	template<typename T = UObject>
	FCinemachineGameplayMessageListenerHandle RegisterListener(FGameplayTag Channel, T* Object, void(T::* Function)(FGameplayTag))
	{
		TWeakObjectPtr<T> WeakObject(Object);
		return RegisterListener(Channel, [WeakObject, Function](FGameplayTag Tag)
			{
				if (T* Object = WeakObject.Get())
				{
					(Object->*Function)(Tag);
				}
			});
	}

	void UnregisterListener(FCinemachineGameplayMessageListenerHandle Handle);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Event")
	void BroadcastMessage(FGameplayTagContainer Channel);

private:
	FCinemachineGameplayMessageListenerHandle RegisterListenerInternal(FGameplayTag Channel, TFunction<void(FGameplayTag)>&& Callback, EGameplayMessageMatch MatchType);

	void UnregisterListenerInternal(FGameplayTag Channel, int32 HandleID);

	void BroadcastMessageInternal(FGameplayTagContainer Channel);

public:
	UPROPERTY(BlueprintAssignable, Category = Cinemachine)
	FCinemachineActivationEvent CameraActivatedEvent;

	UPROPERTY(BlueprintAssignable, Category = Cinemachine)
	FCinemachineCameraEvent OnCameraDeactivatedEvent;

	UPROPERTY(BlueprintAssignable, Category = Cinemachine)
	FCinemachineBrainEvent OnCameraUpdatedEvent;

	UPROPERTY(BlueprintAssignable, Category = Cinemachine)
	FCinemachineBlendEvent OnBlendCreatedEvent;

	UPROPERTY(BlueprintAssignable, Category = Cinemachine)
	FCinemachineCameraEvent BlendFinishedEvent;

private:
	UPROPERTY()
	TArray<UCinemachineVirtualCameraBaseComponent*> ActiveCameras;

	UPROPERTY()
	TArray<FCVCameraArray> AllCameras;

	UPROPERTY()
	UCinemachineVirtualCameraBaseComponent* RoundRobinVCameraLastFrame = nullptr;

	UPROPERTY()
	TArray<UCinemachineBrainComponent*> ActiveBrains;

	float LastUpdateTime = 0.0F;

	bool bActiveCamerasAreSorted = false;

	int32 ActivationSequence = 0;

	TMap<FGameplayTag, FChannelListenerList> ListenerMap;
};
