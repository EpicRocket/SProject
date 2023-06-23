

#pragma once

#include "Camera/Component/CinemachineTransposerComponent.h"
#include "Camera/CinemachineAxisState.h"
#include "CinemachineOrbitalTransposerComponent.generated.h"

UENUM(BlueprintType)
enum class ECinemachineOrbitalTransposerHeadingDefinition
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
struct FCinemachineOrbitalTransposerHeading
{
	GENERATED_USTRUCT_BODY()

public:
	FCinemachineOrbitalTransposerHeading()
	{
	}

	FCinemachineOrbitalTransposerHeading(ECinemachineOrbitalTransposerHeadingDefinition InDefinition, int32 InVelocityFilterStrength, float InBias)
		: Definition(InDefinition)
		, VelocityFilterStrength(InVelocityFilterStrength)
		, Bias(InBias)
	{
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	ECinemachineOrbitalTransposerHeadingDefinition Definition = ECinemachineOrbitalTransposerHeadingDefinition::TargetForward;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0, ClampMax = 10))
	int32 VelocityFilterStrength = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -180.0, ClampMax = 180.0))
	float Bias = 0.0F;
};

UCLASS(Blueprintable, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineOrbitalTransposerComponent : public UCinemachineTransposerComponent
{
	GENERATED_BODY()

public:
	UCinemachineOrbitalTransposerComponent();

    //~ Begin USceneComponent Interface
	virtual void BeginPlay() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	//~ End USceneComponent Interface

	//~ Begin UCinemachineBaseComponent interface
	virtual void OnTargetObjectWarped(USceneComponent* Target, FVector LocationDelta) override;
	virtual void ForceCameraLocation(FVector Location, FRotator Rotation) override;
	virtual bool OnTransitionFromCamera(ICinemachineCameraInterface* FromCamera, FVector WorldUp, float DeltaTime, FCinemachineTransitionParameters& TransitionParameters) override;
	virtual void MutateCameraState(FCinemachineCameraState& State, float DeltaTime) override;
	virtual bool RequriesUserInput() const override
	{
		return true;
	}
	//~ End UCinemachineBaseComponent interface

	//~ Begin UCinemachineTransposerComponent interface
	virtual FVector GetTargetCameraLocation(FVector WorldUp) override;
	//~ End UCinemachineTransposerComponent interface

	float UpdateHeading(float DeltaTime, FVector Up, FCinemachineAxisState& OutAxis);

	float UpdateHeading(float DeltaTime, FVector Up, FCinemachineAxisState& OutAxis, FCinemachineAxisStateRecentering& OutRecentering, bool bIsLive);

	void UpdateInputAxisProvider();

	float GetAxisClosestValue(FVector CameraLocation, FVector Up);

private:
	float InternalUpdateHeading(UCinemachineOrbitalTransposerComponent* Orbital, float DeltaTime, FVector Up);

	float GetTargetHeading(float CurrentHeading, FRotator TargetOrientation);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Transposer")
	FCinemachineOrbitalTransposerHeading Heading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Transposer")
	FCinemachineAxisStateRecentering RecenterToTargetHeading;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Transposer")
    FCinemachineAxisState XAxis;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cinemachine|Component|Transposer")
	bool bHeadingIsSlave;
    
	TFunction<float(UCinemachineOrbitalTransposerComponent*, float, FVector)> HeadingUpdater;

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
