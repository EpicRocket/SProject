

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Components/SceneComponent.h"
#include "CinemachineCameraState.h"
#include "CinemachineCameraInterface.generated.h"

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCinemachineCameraInterface : public UInterface
{
	GENERATED_BODY()
};

class ICinemachineCameraInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual FString GetCameraName() const
	{ /* Needs Overriding */
		return FString(TEXT("Unknown Camera"));
	}

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual int32 GetPriority() const
	{ /* Needs Overriding */
		return 0;
	}

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual void SetPriority(int32 InValue) = 0;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual USceneComponent* GetLookAt() const = 0;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual void SetLookAt(USceneComponent* LookAtSceneComponent) = 0;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual USceneComponent* GetFollow() const = 0;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual void SetFollow(USceneComponent* FollowSceneComponent) = 0;

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual FCinemachineCameraState GetState() const = 0;

	virtual ICinemachineCameraInterface* GetParentCamera()
	{ /* Needs Overriding */
		return nullptr;
	}

	/** 이 CV카메라가 라이브 중인 카메라의 자식인지 여부 */
	virtual bool IsLiveChild(ICinemachineCameraInterface* InCamera, bool DominantChildOnly = false) const
	{ /* Needs Overriding */
		return false;
	}

	/** CV카메라 상태 갱신 */
	virtual void UpdateCameraState(FVector WorldUp, float DeltaTime)
	{ /* Needs Overriding */
	}

	/** CV카메라 상태 갱신, 프레임 당 한번 만 호출되도록 제한이 되어 있음 (외부 호출 금지) */
	virtual void InternalUpdateCameraState(FVector WorldUp, float DeltaTime)
	{ /* Needs Overriding */
	}

	/** 새 CV카메라가 활성화 되었을 때 호출 */
	virtual void OnTransitionFromCamera(ICinemachineCameraInterface* InFromCamera, FVector WorldUp, float DeltaTime)
	{ /* Needs Overriding */
	}
};
