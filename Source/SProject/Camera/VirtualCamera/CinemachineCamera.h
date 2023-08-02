

#include "Camera/Core/CinemachineVirtualCameraBaseComponent.h"
#include "CinemachineCamera.generated.h"

class UCinemachineBaseStage;

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Cinemachine), HideCategories = (Navigation, HOLD, Rendering, PathTracing, Modible, Physics, Collision, RayTracing, Cooking, LOD), meta = (BlueprintSpawnableComponent))
class UCinemachineCamera : public UCinemachineVirtualCameraBaseComponent
{
	GENERATED_BODY()

public:
	UCinemachineCamera();

	//~ Begin USceneComponent Interface.
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent) override;
#endif
	//~ End USceneComponent Interface.

	//~Begin UCinemachineVirtualCameraBaseComponent
	virtual USceneComponent* GetFollow() const override;
	virtual void SetFollow(USceneComponent* FollowSceneComponent) override;
	virtual USceneComponent* GetLookAt() const override;
	virtual void SetLookAt(USceneComponent* LookAtSceneComponent) override;
	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime) override;
	virtual void OnTransitionFromCamera(TScriptInterface<ICinemachineCameraInterface> FromCamera, FVector WorldUp, float DeltaTime) override;
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;
	virtual float GetMaxDampTime() override;
	virtual UCinemachineBaseStage* GetStage(ECVStageType Stage) override;
	//~ End UCinemachineVirtualCameraBaseComponent

protected:
	//~Begin UCinemachineVirtualCameraBaseComponent
	virtual void OnInitailize() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	//~ End UCinemachineVirtualCameraBaseComponent

private:
	FCinemachineCameraState InvokePipeline(FCinemachineCameraState& OutState, float DeltaTime);

public:
	UPROPERTY(BlueprintReadOnly, Category = Cinemachine)
	USceneComponent* TrackingTarget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Cinemachine)
	USceneComponent* LookAtTarget = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Cinemachine, meta = (DisplayName = "카메라 렌즈 설정"))
	FCinemachineLensSettings LensSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|State", meta = (Bitmask, BitmaskEnum = "/Script/Ccg.ECVBlendHint"))
	int32 BlendHint = 0;

private:
	/** Body→Aim→Noise→Finalize 순서가 보장되어야 합니다. */
	UPROPERTY()
	TArray<UCinemachineBaseStage*> Pipeline;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Cinemachine|Body", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCinemachineBaseStage> BodyStage;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Cinemachine|Body", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCinemachineBaseStage> AimStage;

	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Cinemachine|Body", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCinemachineBaseStage> NoiseStage;
};
