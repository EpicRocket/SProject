

#pragma once

#include "Camera/CinemachineExtension.h"
#include "CinemachineLockonExtension.generated.h"

USTRUCT(BlueprintType)
struct FCVLockonExtensionData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxRadius = 600.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 180.0))
	float MinAngle = 80.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 180.0))
	float MaxAngle = 150.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float InterpSpeed = 1.0F;
};

UCLASS(BlueprintType, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineLockonExtension : public UCinemachineExtension
{
    GENERATED_BODY()

protected:
    //~ Begin UCinemachineExtension
	virtual void PostPipelineStageCallback(UCinemachineVirtualCameraBaseComponent* VCamera, ECVStage Stage, FCinemachineCameraState& State, float DeltaTime) override;
    //~ End UCinemachineExtension

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Extension")
	FCVLockonExtensionData LockonData;
};
