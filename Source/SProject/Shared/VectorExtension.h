

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
	UFUNCTION(BlueprintPure, Category = "VectorExtension")
	static FVector Slerp(const FVector& A, const FVector& B, float Alpha)
	{
		// If either vector is a zero vector, return the other vector
		if (A.IsNearlyZero())
		{
			return B;
		}
		if (B.IsNearlyZero())
		{
			return A;
		}

		// Get the normalized vectors and their dot product
		FVector NormalizedA = A.GetSafeNormal();
		FVector NormalizedB = B.GetSafeNormal();
		float DotProduct = FVector::DotProduct(NormalizedA, NormalizedB);

		// Clamp the dot product to the range -1 to 1
		DotProduct = FMath::Clamp(DotProduct, -1.0f, 1.0f);

		// If the vectors are nearly parallel, return one of them
		if (FMath::IsNearlyEqual(FMath::Abs(DotProduct), 1.0f))
		{
			return NormalizedA;
		}

		// Calculate the angle and its sine
		const float Omega = FMath::Acos(DotProduct);
		const float SinOmega = FMath::Sin(Omega);

		// Calculate the terms and return the result
		const FVector TermOne = A * (FMath::Sin(Omega * (1.0f - Alpha)) / SinOmega);
		const FVector TermTwo = B * (FMath::Sin(Omega * Alpha) / SinOmega);
		return TermOne + TermTwo;
	}

	UFUNCTION(BlueprintPure, Category = VectorExtension)
	static float Angle(FVector V1, FVector V2)
	{
		V1.Normalize();
		V2.Normalize();
		return FMath::RadiansToDegrees(FMath::Atan2((V1 - V2).SizeSquared(), (V1 + V2).SizeSquared())) * 2.0F;
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
		FVector Axis = V1.GetSafeNormal() ^ V2.GetSafeNormal();
		if (Axis.IsNearlyZero())
		{
			Axis = Up;
		}
		return FQuat(Axis, Angle(V1, V2));
	}

	UFUNCTION(BlueprintPure, Category = VectorExtension)
	static FVector Bezier3(FVector V0, FVector V1, FVector V2, FVector V3, float Alpha)
	{
		Alpha = FMath::Clamp(Alpha, 0.0F, 1.0F);
		float OneMinusAlpha = 1.0F - Alpha;
		return OneMinusAlpha * OneMinusAlpha * OneMinusAlpha * V0 + 3.0F * OneMinusAlpha * OneMinusAlpha * Alpha * V1 + 3.0F * OneMinusAlpha * Alpha * Alpha * V2 + Alpha * Alpha * Alpha * V3;
	}

	UFUNCTION(BlueprintPure, Category = VectorExtension)
	static float ClosestPointOnSegment(FVector V1, FVector S1, FVector S2)
	{
		FVector Segment = S2 - S1;
		if (Segment.IsNearlyZero())
		{
			return 0.0F;
		}
		return FMath::Clamp((V1 - S1) | Segment / Segment.SizeSquared(), 0.0F, 1.0F);
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
