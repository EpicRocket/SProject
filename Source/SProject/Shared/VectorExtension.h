

#pragma once
#include "Kismet/BlueprintFunctionLibrary.h"
#include "VectorExtension.generated.h"

/**
 * Vector 타입의 확장 기능을 제공하기 위한 클래스입니다.
 */
UCLASS(meta = (BlueprintThreadSafe))
class UVectorExtension : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = VectorExtension)
	static float Angle(FVector V1, FVector V2)
	{
		V1.Normalize();
		V2.Normalize();
		return FMath::RadiansToDegrees(FMath::Atan2((V1 - V2).Size(), (V1 + V2).Size())) * 2.0F;
	}

	UFUNCTION(BlueprintPure, Category = VectorExtension)
	static float SignedAngle(FVector V1, FVector V2, FVector Up)
	{
		float Angle = UVectorExtension::Angle(V1, V2);
		return FMath::Sign(Up | (V1 ^ V2)) < 0.0F ? -Angle : Angle;
	}

	UFUNCTION(BlueprintPure, Category = VectorExtension)
	static FQuat SafeFromToRotation(FVector V1, FVector V2, FVector Up)
	{
		FVector P1 = FVector::VectorPlaneProject(V1, Up);
		FVector P2 = FVector::VectorPlaneProject(V2, Up);
		if (P1.IsNearlyZero() || P2.IsNearlyZero())
		{
			FVector Axis = V1 ^ V2;
			if (Axis.IsNearlyZero())
			{
				Axis = Up;
			}
			return FQuat(Axis, FMath::DegreesToRadians(UVectorExtension::Angle(V1, V2)));
		}
		float PitchChange = FMath::DegreesToRadians(UVectorExtension::Angle(V2, Up) - UVectorExtension::Angle(V1, Up));
		return FQuat(Up, FMath::DegreesToRadians(UVectorExtension::SignedAngle(P1, P2, Up))) * FQuat((Up ^ V1).GetSafeNormal(), PitchChange);
	}

	UFUNCTION(BlueprintPure, Category = VectorExtension)
	static float ClosestPointOnSegment(FVector PointSpace, FVector StartSegment, FVector EndSegment)
	{
		const FVector Segment = EndSegment - StartSegment;
		const float Length = Segment.SizeSquared();
		if (Length < UE_KINDA_SMALL_NUMBER)
		{
			return 0.0F;
		}
		return FMath::Clamp(((PointSpace - StartSegment) | Segment) / Length, 0.0F, 1.0F);
	}

	UFUNCTION(BlueprintPure, Category = VectorExtension)
	static FRotator LookRotation(FVector Forward, FVector Up = FVector::UpVector)
	{
		Forward.Normalize();
		Up.Normalize();
		FVector Right = (Up ^ Forward).GetSafeNormal();
		Up = Forward ^ Right;
		return FQuat(FMatrix(Forward, Right, Up, FVector::ZeroVector)).Rotator();
	}

	/** true is left, false is right (so zero value is right) */
	UFUNCTION(BlueprintPure, Category = VectorExtension)
	static bool IsLeftDirection(FVector ForwardVector, FVector TargetDirection, FVector WorldUp = FVector::UpVector)
	{
		FVector ForwardOnPlane = ForwardVector - WorldUp * (ForwardVector | WorldUp);
		FVector VectorAToBOnPlane = TargetDirection - WorldUp * (TargetDirection | WorldUp);

		ForwardOnPlane.Normalize();
		VectorAToBOnPlane.Normalize();

		FVector CrossProduct = ForwardOnPlane ^ VectorAToBOnPlane;
		float DotProductWithWorldUp = FVector::DotProduct(CrossProduct, WorldUp);
		return DotProductWithWorldUp > 0;
	}
};
