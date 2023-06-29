

#pragma once

#include "CoreMinimal.h"
#include "CinemachineTargetGroupBaseComponent.h"
#include "CinemachineTargetGroupComponent.generated.h"

USTRUCT(BlueprintType)
struct FCinemachineTargetGroupMember
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* Target = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Weight = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Radius = 0.0F;
};

UENUM(BlueprintType)
enum class ECinemachineTargetGroupLocationMode : uint8
{
	GroupCenter,
	GroupAverage,
};

UENUM(BlueprintType)
enum class ECinemachineTargetGroupRotationMode : uint8
{
	Manual,
	GroupAverage,
};

/**
 * 가상 카메라를 그룹으로 묶어 가중치와 반경에 따른 결과 값을 출력하는 컴포넌트
*/
UCLASS()
class UCinemachineTargetGroupComponent : public UCinemachineTargetGroupBaseComponent
{
    GENERATED_BODY()

public:
	UCinemachineTargetGroupComponent();

	//~ Begin UActorComponent interface
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent interface

	//~ Begin UCinemachineTargetGroupBaseComponent interface
	virtual FBox GetBoundingBox() override;
	virtual FSphere GetBoundingSphere() override;
	virtual bool IsEmpty() override;
	virtual FBox GetViewSpaceBoundingBox(FMatrix Observer) override;
	virtual void GetViewSpaceAngularBounds(FMatrix Observer, FVector2D& MinAngles, FVector2D& MaxAngles, FVector2D ZRange) override;
	//~ End UCinemachineTargetGroupBaseComponent interface

	void DoUpdate();

	void AddMember(USceneComponent* Target, float Weight, float Radius);

	void RemoveMember(USceneComponent* Target);

	FSphere GetWeightedBoundsForMember(int32 MemberIndex);

	FSphere WeightedMemberBoundsForValidMember(FCinemachineTargetGroupMember& OutMember, FVector TargetLocation, float Max);

	void UpdateMemberValidity();

	FVector CalculateAverageLocation(float& OutMax);

	FBox CalculateBoundingBox();

	FSphere CalculateBoundingSphere(float Max);

	FRotator CalculateAverageOrientation();

private:
	bool CachedCountIsValid() const;

	bool IndexIsValid(int32 Index) const;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineTargetGroup)
	ECinemachineTargetGroupLocationMode LocationMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineTargetGroup)
	ECinemachineTargetGroupRotationMode RotationMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineTargetGroup)
	TArray<FCinemachineTargetGroupMember> Members;

private:
	float MaxWeight;		// CalculateAverageLocation 호출을 통해 값 갱신됨
	FVector AverageLocation;
	FBox BoundingBox;
	FSphere BoundingSphere;
	uint64 LastUpdateFrame;
	TArray<int32> ValidMembers;
	TArray<bool> MemberValidity;
};
