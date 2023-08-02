

#pragma once

#include "Camera/Core/CinemachineVirtualCameraBaseComponent.h"
#include "AlphaBlend.h"
#include "Camera/Interface/CinemachineMixerInterface.h"
#include "CinemachineCameraManager.generated.h"

UCLASS()
class UCinemachineCameraManager : public UCinemachineVirtualCameraBaseComponent, public ICinemachineMixerInterface
{
    GENERATED_BODY()

public:
	//~Begin UCinemachineVirtualCameraBaseComponent
	virtual USceneComponent* GetFollow() const override;
	virtual void SetFollow(USceneComponent* FollowSceneComponent) override;
	virtual USceneComponent* GetLookAt() const override;
	virtual void SetLookAt(USceneComponent* LookAtSceneComponent) override;
	virtual void SetPreviousStateIsValid(bool bValue) override;
	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime) override;
	virtual void OnTransitionFromCamera(TScriptInterface<ICinemachineCameraInterface> FromCamera, FVector WorldUp, float DeltaTime) override;
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;
	//~ End UCinemachineVirtualCameraBaseComponent
	
	//~ Begin ICinemachineMixerInterface
	virtual bool IsLiveChild(ICinemachineCameraInterface* ICamera, bool DominantChildOnly = false) const override;
	//~ End ICinemachineMixerInterface

	TScriptInterface<ICinemachineCameraInterface> GetLiveChild();

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Blend")
	bool IsBlending() const;

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Blend")
	UCinemachineBlend* GetActiveBlend() const;

	virtual UCinemachineVirtualCameraBaseComponent* ChooseCurrentCamera(FVector WorldUp, float DeltaTime) { return nullptr;/*Needs Overrding*/ }

	UFUNCTION(BlueprintPure, Category = Cinemachine)
	TArray<UCinemachineVirtualCameraBaseComponent*> GetChildCameras();

protected:
	//~ Begin UCinemachineVirtualCameraBaseComponent
	virtual void OnInitailize() override;
	virtual void OnEnable() override;
	virtual void OnDisable() override;
	//~ End UCinemachineVirtualCameraBaseComponent

	void SetLiveChild(ICinemachineCameraInterface* ActiveCamera, FVector WorldUp, float DeltaTime);

	void ResetLiveChild();

	void FinalizeCameraState(float DeltaTime);

public:
	UPROPERTY(BlueprintReadOnly, Category = Cinemachine)
	USceneComponent* TrackingTarget = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Cinemachine)
	USceneComponent* LookAtTarget = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cinemachine|Blend")
	FAlphaBlend DefaultBlend;

private:
	UPROPERTY()
	TArray<UCinemachineVirtualCameraBaseComponent*> ChildCameras;

	UPROPERTY()
	class UCinemachineBlendManager* BlendManager = nullptr;

	ICinemachineCameraInterface* TransitionFrom = nullptr;
};
