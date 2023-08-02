

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "CinemachineTargetGroupComponent.generated.h"

USTRUCT(BlueprintType)
struct FCVTargetGroupMember
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
enum class ECVTargetGroupLocationMode : uint8
{
	GroupCenter,
	GroupAverage,
};

UENUM(BlueprintType)
enum class ECVTargetGroupRotationMode : uint8
{
	Manual,
	GroupAverage,
};

/**
 * 가상 카메라를 그룹으로 묶어 가중치와 반경에 따른 결과 값을 출력하는 컴포넌트
 */
UCLASS(BlueprintType, ClassGroup = (Cinemachine), meta = (BlueprintSpawnableComponent))
class UCinemachineTargetGroupComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UCinemachineTargetGroupComponent();

	//~ Begin UActorComponent interface
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	//~ End UActorComponent interface

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual FBox GetBoundingBox();

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual FSphere GetBoundingSphere();

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual bool IsEmpty();

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual FBox GetViewSpaceBoundingBox(FMatrix Observer);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Interface")
	virtual void GetViewSpaceAngularBounds(FMatrix Observer, FVector2D& MinAngles, FVector2D& MaxAngles, FVector2D ZRange);

	void DoUpdate();

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Group")
	void ClearMembers();

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Group")
	void AddMember(USceneComponent* Target, float Weight, float Radius);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Group")
	void InsertMember(USceneComponent* Target, int32 Position, float Weight, float Radius);

	UFUNCTION(BlueprintCallable, Category = "Cinemachine|Group")
	void RemoveMember(USceneComponent* Target);

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Group")
	FSphere GetWeightedBoundsForMember(int32 MemberIndex);

	UFUNCTION(BlueprintPure, Category = "Cinemachine|Group")
	FSphere WeightedMemberBoundsForValidMember(FCVTargetGroupMember& OutMember, FVector TargetLocation, float Max);

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
	ECVTargetGroupLocationMode LocationMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineTargetGroup)
	ECVTargetGroupRotationMode RotationMode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = CinemachineTargetGroup)
	TArray<FCVTargetGroupMember> Members;

private:
	float MaxWeight; // CalculateAverageLocation 호출을 통해 값 갱신됨
	FVector AverageLocation;
	FBox BoundingBox;
	FSphere BoundingSphere;
	uint64 LastUpdateFrame;
	TArray<int32> ValidMembers;
	TArray<bool> MemberValidity;
};
