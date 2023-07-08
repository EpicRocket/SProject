

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CinemachineCameraInterface.h"
#include "CinemachineCameraState.h" // include lens settings
#include "CinemachineBlend.h"
#include "CinemachineVirtualCameraBaseComponent.generated.h"

class USceneComponent;
class UCinemachineExtension;
class UCinemachineBlend;
class UCinemachineTargetGroupBaseComponent;
class UCinemachineTargetGroupComponent;
class ICinemachineInputAxisProviderInterface;
enum class ECVStage : uint8;
enum class ECVBlendHint : uint8;

#define CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_PROPERTY(PropertyChangedEvent, CurrentClassType, StageClass, StageTemplate, StageType) \
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(CurrentClassType, StageClass)) { \
		if (IsTemplate()) { \
			SetStageTemplate(StageClass, StageClass, StageTemplate, StageType); \
		} \
		else { \
			CurrentClassType* Archetype = CastChecked<CurrentClassType>(GetArchetype()); \
			StageTemplate = (Archetype->StageClass == StageClass ? Archetype->StageTemplate : nullptr); \
		} \
	}

#define CINEMACHINE_SPAWN_TEMPLATE_POST_CHANGE_CHAIN_PROPERTY(PropertyChangedEvent, CurrentClassType, StageClass, StageTemplate, StageType) \
	if (PropertyChangedEvent.Property && PropertyChangedEvent.Property->GetFName() == GET_MEMBER_NAME_CHECKED(CurrentClassType, StageClass)) { \
		if (IsTemplate()) { \
			SetStageTemplate(StageClass, StageClass, StageTemplate, StageType); \
		} \
		else { \
			StageTemplate = CastChecked<CurrentClassType>(GetArchetype())->StageTemplate; \
		} \
	}


UENUM(BlueprintType)
enum class ECVStandbyUpdateMode : uint8
{
	Nerver UMETA(DisplayName = "카메라 갱신하지 않음"),
	Always UMETA(DisplayName = "카메라 항상 갱신"),
	RoundRobin UMETA(DisplayName = "Tick 당 한번 갱신"),
};

UCLASS(Abstract, BlueprintType, ClassGroup = (Cinemachine))
class UCinemachineVirtualCameraBaseComponent : public USceneComponent, public ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	UCinemachineVirtualCameraBaseComponent();

	// ~Begin AActor
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	// ~End AActor

	//~ Begin ICinemachineCameraInterface
	virtual FString GetCameraName() const override
    {
		return GetName();
	}

	virtual int32 GetPriority() const override
    {
        return Priority;
    }

	virtual void SetPriority(int32 InValue) override
    {
        Priority = InValue;
    }

	virtual USceneComponent* GetLookAt() const override;

	virtual void SetLookAt(USceneComponent* LookAtSceneComponent) override;

	virtual USceneComponent* GetFollow() const override;

	virtual void SetFollow(USceneComponent* FollowSceneComponent) override;

	virtual FCinemachineCameraState GetState() const override
    {
        return VCameraState;
    }

	virtual ICinemachineCameraInterface* GetParentCamera() override;

	virtual void UpdateCameraState(FVector WorldUp, float DeltaTime) override;

	virtual void OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime) override;

	//~ End ICinemachineCameraInterface

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void Init();

    UFUNCTION(BlueprintCallable, Category = Cinemachine)
    void SetEnable(bool bEnable);

    UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsEnable() const
    {
        return bCameraEnable;
    }

    virtual bool GetPreviousStateIsValid() const
	{
		return PreviousStateIsValid;
	}

	virtual void SetPreviousStateIsValid(bool bValue)
	{
		PreviousStateIsValid = bValue;
	}

    UFUNCTION(BlueprintPure, Category = Cinemachine)
   	virtual bool RequiresUserInput();

    UFUNCTION(BlueprintPure, Category = Cinemachine)
	virtual float GetMaxDampTime();

    float DetachedFollowTargetDamp(float Initial, float DampTime, float DeltaTime) const;

	FVector DetachedFollowTargetDamp(FVector Initial, FVector DampTime, float DeltaTime) const;

	FVector DetachedFollowTargetDamp(FVector Initial, float DampTime, float DeltaTime) const;

	float DetachedLookAtTargetDamp(float Initial, float DampTime, float DeltaTime) const;

	FVector DetachedLookAtTargetDamp(FVector Initial, FVector DampTime, float DeltaTime) const;

	FVector DetachedLookAtTargetDamp(FVector Initial, float DampTime, float DeltaTime) const;

    UFUNCTION(BlueprintPure, Category = Cinemachine)
	USceneComponent* ResolveFollow(USceneComponent* LocalFollow) const;

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	USceneComponent* ResolveLookAt(USceneComponent* LocalLookAt) const;

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void MoveToTopOfPrioritySubQueue();

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void AddExtension(UCinemachineExtension* Extension);

	UFUNCTION(BlueprintCallable, Category = Cinemachine)
	void RemoveExtension(UCinemachineExtension* Extension);

	virtual UCinemachineTargetGroupBaseComponent* GetFollowTargetGroup();

	virtual UCinemachineVirtualCameraBaseComponent* GetFollowTargetAsVCamera();

	virtual UCinemachineTargetGroupBaseComponent* GetLookAtTargetGroup();

	virtual UCinemachineVirtualCameraBaseComponent* GetLookAtTargetAsVCamera();

	ICinemachineInputAxisProviderInterface* GetInputAxisProvider() const;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Input")
	void SetInputAxisProvider(TScriptInterface<ICinemachineInputAxisProviderInterface> InsertProvider);

	virtual void UpdateInputAxisProvider() { /*Needs Overridng*/ }

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	bool IsInitialized() const
	{
		return bIsInitialized;
	}

protected:
	// ~Begin AActor
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);
	// ~End AActor

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = Cinemachine)
	void OnInitailize();

	virtual void OnInitailize_Implementation() { /* Needs Overrding */ }

    virtual void OnEnable();

	virtual void OnDisable();

	void InvokePostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECVStage Stage, FCinemachineCameraState& State, float DeltaTime);

	void InvokePrePipelineMutateCameraStateCallback(UCinemachineVirtualCameraBaseComponent* VCamera, FCinemachineCameraState& State, float DeltaTime);

	bool InvokeOnTransitionInExtensions(UCinemachineVirtualCameraBaseComponent* FromVCamera, FVector WorldUp, float DeltaTime);

	void ApplyLocationBlendMethod(FCinemachineCameraState& State, ECVBlendHint BlendHint);

	UCinemachineBlend* CreateBlend(UObject* CameraA, UObject* CameraB, FCinemachineBlendDefinition BlendDefinition, UCinemachineBlend* ActiveBlend);

	FCinemachineCameraState PullStateFromVirtualCamera(FVector WorldUp, FCinemachineLensSettings& Lens);

	void UpdateTargetCache();

private:
    void UpdateSlaveStatus();

	void UpdateVcamPoolStatus();

	void InvalidateCachedTargets();

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cinemachine)
	int32 Priority = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cinemachine)
	int32 ActivationId = INDEX_NONE;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cinemachine, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float FollowTargetAttachment = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cinemachine, meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float LookAtTargetAttachment = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	ECVStandbyUpdateMode StandbyUpdateMode = ECVStandbyUpdateMode::RoundRobin;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cinemachine)
	TArray<UCinemachineExtension*> Extensions;

	bool bFollowTargetChanged = false;

	bool bLookAtTargetChanged = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	FCinemachineLensSettings VCameraLens;

protected:
	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadWrite, Category = Cinemachine)
	USceneComponent* LookAtTarget = nullptr;

	UPROPERTY(VisibleAnywhere, Transient, BlueprintReadWrite, Category = Cinemachine)
	USceneComponent* FollowTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Cinemachine)
	FCinemachineCameraState VCameraState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Cinemachine)
	UObject* InputAxisProvider = nullptr;

	UPROPERTY(EditAnywhere, Category = Cinemachine)
	bool bCameraEnable = false;

	void SetStageTemplate(TSubclassOf<class UCinemachineBaseStage>& OutClass, UClass* InClass, TObjectPtr<class UCinemachineBaseStage>& OutTemplate, ECVStage Filter);

private:
	bool bIsInitialized = false;

	bool PreviousStateIsValid = false;

	bool SlaveStatusUpdated = false;

	int32 QueuePriority = MAX_int32;

	float BlendStartPosition = 0.0F;

	UPROPERTY(Transient)
	UCinemachineVirtualCameraBaseComponent* ParentCamera = nullptr;

	UPROPERTY(Transient)
	USceneComponent* CachedFollowTarget = nullptr;

	UPROPERTY(Transient)
	UCinemachineVirtualCameraBaseComponent* CachedFollowTargetVCamera = nullptr;

	UPROPERTY(Transient)
	UCinemachineTargetGroupComponent* CachedFollowTargetGroup = nullptr;

	UPROPERTY(Transient)
	USceneComponent* CachedLookAtTarget = nullptr;

	UPROPERTY(Transient)
	UCinemachineVirtualCameraBaseComponent* CachedLookAtTargetVCamera = nullptr;

	UPROPERTY(Transient)
	UCinemachineTargetGroupComponent* CachedLookAtTargetGroup = nullptr;

	bool bCacheCameraEnable = false;
};
