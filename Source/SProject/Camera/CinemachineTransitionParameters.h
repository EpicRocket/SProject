

#pragma once

#include "CoreMinimal.h"
#include "CinemachineTransitionParameters.generated.h"

class UCinemachineVirtualCameraBaseComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FCinemachineVirtualCameraActivatedEvent, UCinemachineVirtualCameraBaseComponent*, VCamera, UCinemachineVirtualCameraBaseComponent*, VCameraFrom);

UENUM(BlueprintType)
enum class ECinemachineBlendHint : uint8
{
	None,
	SphericalLocation,
	CylindriacalLocation,
	ScreenSpaceAimWhenTargetsDiffer
};

/**
 * 가상 카메라가 다음 카메라로 전환 하는 방식에 대한 매개 변수 구조체
 */
USTRUCT(BlueprintType)
struct FCinemachineTransitionParameters
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	ECinemachineBlendHint BlendHint = ECinemachineBlendHint::None;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Cinemachine)
	bool bInheritLocation = false;

	UPROPERTY(BlueprintAssignable, BlueprintReadWrite, Category = Cinemachine)
	FCinemachineVirtualCameraActivatedEvent CameraActivatedEvent;
};
