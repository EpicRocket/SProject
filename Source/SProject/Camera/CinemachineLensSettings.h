
#pragma once
#include "CoreMinimal.h"
#include "CinemachineLensSettings.generated.h"

UENUM(BlueprintType)
enum class ECinemachineLensSettginsOverrideModes : uint8
{
    None,
    Orthographic,
    Perspective,
    Physical
};

UCLASS(BlueprintType)
class SPROJECT_API UCinemachineLensSettings : public UObject
{
    GENERATED_BODY()

public:

    static UCinemachineLensSettings FromCamera(class UCameraComponent* CameraComponent);

    UFUNCTION(BlueprintPure, Category = "Cinemachine Lens Settings")
    bool IsOrthographic() const
    {
        return ModeOverride == ECinemachineLensSettginsOverrideModes::Orthographic || ModeOverride == ECinemachineLensSettginsOverrideModes::None && bOrthographic;
    }

    bool IsPhysicalCamera() const
    {
        return ModeOverride == ECinemachineLensSettginsOverrideModes::Physical || ModeOverride == ECinemachineLensSettginsOverrideModes::None && bPhysicalCamera;
    }    

    UFUNCTION(BlueprintPure, Category = "Cinemachine Lens Settings")
	FVector2D GetSensorSize() const
    {
        return SensorSize;
    }

    UFUNCTION(BlueprintCallable, Category = "Cinemachine Lens Settings")
    void SetSensorSize(FVector2D InSensorSize)
    {
        SensorSize = InSensorSize;
    }

    UFUNCTION(BlueprintPure, Category = "Cinemachine Lens Settings")
    float GetAspect() const
    {
        return FMath::IsNearlyZero(SensorSize.Y) ? 1.0F : SensorSize.X / SensorSize.Y;
    }

	void SnapshotCameraReadOnlyProperties(class UCameraComponent* CameraComponent);

public:
    static UCinemachineLensSettings Default;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine Lens Settings", meta = (ClampMin = "1.0", ClampMax = "179.0"))
    float FieldOfView;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine Lens Settings")
    float OrthographicSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine Lens Settings")
    float NearClipPlane;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine Lens Settings")
    float FarClipPlane;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine Lens Settings", meta = (ClampMin = "-180.0", ClampMax = "180.0"))
    float Dutch;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine Lens Settings")
    ECinemachineLensSettginsOverrideModes ModeOverride;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine Lens Settings", BlueprintGetter = "GetSensorSize", BlueprintSetter = "SetSensorSize")
    FVector2D SensorSize;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine Lens Settings")
    bool bOrthographic;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine Lens Settings")
    bool bPhysicalCamera;
};
