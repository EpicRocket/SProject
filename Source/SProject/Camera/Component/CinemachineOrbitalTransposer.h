

#pragma once

#include "Camera/Component/CinemachineTransposer.h"
#include "Camera/CinemachineAxisState.h"
#include "CinemachineOrbitalTransposer.generated.h"

UENUM(BlueprintType)
enum class ECVOrbitalTransposerHeadingDef
{
	// 마지막으로 갱신된 위치와 현재 위치의 차이로 계산
	LocationDelta,
	// Movement Component에서 계산된 Velocity(속도) 값으로 계산
	Velocity,
	// 지정된 타겟이 바라보고 있는 각도로 계산
	TargetForward,
	// 월드 좌표계로 계산
	WorldForward,
};

USTRUCT(BlueprintType)
struct FCVOrbitalTransposerHeading
{
	GENERATED_BODY()

public:
	FCVOrbitalTransposerHeading()
	{
	}

	FCVOrbitalTransposerHeading(ECVOrbitalTransposerHeadingDef InDefinition, int32 InVelocityFilterStrength, float InBias)
		: Definition(InDefinition)
		, VelocityFilterStrength(InVelocityFilterStrength)
		, Bias(InBias)
	{
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECVOrbitalTransposerHeadingDef Definition = ECVOrbitalTransposerHeadingDef::TargetForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 10))
	int32 VelocityFilterStrength = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180.0, ClampMax = 180.0))
	float Bias = 0.0F;
};

USTRUCT(BlueprintType)
struct FCVOrbitalTransposerData
{
	GENERATED_BODY()

public:
	FCVOrbitalTransposerData()
		: Heading(ECVOrbitalTransposerHeadingDef::TargetForward, 4, 0.0F)
		, RecenterToTargetHeading(true, 1.0F, 2.0F)
		, XAxis(-180.0F, 180.0F, true, false, 300.0F, 0.1F, 0.1F)
	{
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCVOrbitalTransposerHeading Heading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FCinemachineAxisStateRecentering RecenterToTargetHeading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FCinemachineAxisState XAxis;
};

UCLASS(BlueprintType, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineOrbitalTransposer : public UCinemachineTransposer
{
	GENERATED_BODY()

public:
	UCinemachineOrbitalTransposer();

    //~ Begin UObject Interface
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End UObject Interface

	//~ Begin UCinemachineBaseStage interface
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;
	virtual bool OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime, FCinemachineTransitionParameters& TransitionParameters) override;
	virtual void MutateCameraState(FCinemachineCameraState& State, float DeltaTime) override;
	virtual bool RequiresUserInput() const override
	{
		return true;
	}
	//~ End UCinemachineBaseStage interface

	//~ Begin UCinemachineTransposer interface
	virtual FVector GetTargetCameraLocation(FVector WorldUp) override;
	//~ End UCinemachineTransposer interface

	float UpdateHeading(float DeltaTime, FVector Up, FCinemachineAxisState& OutAxis);

	float UpdateHeading(float DeltaTime, FVector Up, FCinemachineAxisState& OutAxis, FCinemachineAxisStateRecentering& OutRecentering, bool bIsLive);

	void UpdateInputAxisProvider();

	float GetAxisClosestValue(FVector CameraLocation, FVector Up);

private:
	float InternalUpdateHeading(UCinemachineOrbitalTransposer* Orbital, float DeltaTime, FVector Up);

	float GetTargetHeading(float CurrentHeading, FRotator TargetOrientation);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Stage")
	FCVOrbitalTransposerData OrbitalTransposerData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Stage")
	bool bHeadingIsSlave = false;
    
	TFunction<float(UCinemachineOrbitalTransposer*, float, FVector)> HeadingUpdater;

private:
	FVector LastTargetLocation = FVector::ZeroVector;

	TSharedPtr<struct FHeadingTracker> HeadingTracker = nullptr;

	UPROPERTY(Transient)
	class UMovementComponent* TargetMovementComponent = nullptr;

	UPROPERTY(Transient)
	USceneComponent* PreviouseTarget = nullptr;
	
	FVector LastCameraLocation = FVector::ZeroVector;

	float LastHeading = 0.0F;
};
