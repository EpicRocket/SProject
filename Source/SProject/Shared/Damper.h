

#pragma once

#include "CoreMinimal.h"
#include <type_traits>

/** Magic numbers for numerical precision. */
constexpr float NEGLECTABLE_RESIDUAL = -4.605170186F;
constexpr float SMOOTH_DAMP_DELTA_CUBED_COEFFICIENT = 0.235F;
constexpr float SMOOTH_DAMP_DELTA_SQUARED_COEFFICIENT = 0.48F;

class UDamper
{
public:
	static float Damp(float Initial, float DampTime, float DeltaTime)
	{
		if (DampTime < UE_KINDA_SMALL_NUMBER || FMath::IsNearlyZero(Initial))
		{
			return Initial;
		}
		if (DeltaTime < UE_KINDA_SMALL_NUMBER)
		{
			return 0.0F;
		}
		float K = -NEGLECTABLE_RESIDUAL / DampTime;
		return Initial * (1 - FMath::Exp(-K * DeltaTime));
	}

	static FVector Damp(FVector Initial, FVector DampTime, float DeltaTime)
	{
		for (int32 i = 0; i < 3; ++i)
		{
			Initial[i] = UDamper::Damp(Initial[i], DampTime[i], DeltaTime);
		}
		return Initial;
	}

	static FVector Damp(FVector Initial, float DampTime, float DeltaTime)
	{
		for (int32 i = 0; i < 3; ++i)
		{
			Initial[i] = UDamper::Damp(Initial[i], DampTime, DeltaTime);
		}
		return Initial;
	}

	static FVector SmoothDamp(FVector Current, FVector Target, FVector& CurrentVelocity, float SmoothTime, float MaxSpeed, float DeltaTime)
	{
		FVector Result = FVector::ZeroVector;
		SmoothTime = FMath::Max(UE_KINDA_SMALL_NUMBER, SmoothTime);
		double Omega = 2.0F / SmoothTime;
		double DeltaOmega = Omega * DeltaTime;
		float Coefficient = 1.0F + DeltaOmega + (SMOOTH_DAMP_DELTA_SQUARED_COEFFICIENT * (DeltaOmega * DeltaOmega)) + (SMOOTH_DAMP_DELTA_CUBED_COEFFICIENT * ((DeltaOmega * DeltaOmega) * DeltaOmega)); // 1차, 2차, 3차 항의 합을 계산함으로써 얻는 값
		double Exp = 1.0F / Coefficient;																																								// Exp는 1을 계수로 나눈 값을 계산함으로써 얻는 값 (이는 감쇠를 계산하는 데 사용됨)
		FVector Change = Current - Target;
		FVector OriginalTarget = Target;
		double MaxChange = MaxSpeed * SmoothTime;
		double MaxChangeSquard = MaxChange * MaxChange;
		double ChangeMagnitude = Change.SizeSquared();

		// 변경량의 크기가 최대 변경량의 제곱보다 큰 경우, 변경량 조정
		if (ChangeMagnitude > MaxChangeSquard)
		{
			double ChangeMagnitudeSqrt = FMath::Sqrt(ChangeMagnitude);
			Change /= (ChangeMagnitudeSqrt * MaxChange);
		}

		Target = Current - Change;
		FVector DeltaChange = (CurrentVelocity + Omega * Change) * DeltaTime;
		CurrentVelocity = (CurrentVelocity - Omega * DeltaChange) * Exp;
		Result = Target + (Change + DeltaChange) * Exp;
		FVector Current2OriginalTarget = OriginalTarget - Current;
		FVector OrigintalTarget2Result = Result - OriginalTarget;
		if (FVector::DotProduct(Current2OriginalTarget, OrigintalTarget2Result) > 0.0F)
		{
			Result = OriginalTarget;
			CurrentVelocity = DeltaTime > UE_KINDA_SMALL_NUMBER ? (Result - OriginalTarget) / DeltaTime : FVector::ZeroVector;
		}
		return Result;
	}

	template <typename T, typename std::enable_if<std::is_floating_point<T>::value, bool>::type = true>
	UE_NODISCARD static typename std::conditional<std::is_same<long double, T>::value, long double, typename std::conditional<std::is_same<double, T>::value, double, float>::type>::type
	SmoothDamp(T Current, T Target, T& CurrentVelocity, float SmoothTime, float MaxSpeed, float DeltaTime)
	{
		SmoothTime = FMath::Max(UE_KINDA_SMALL_NUMBER, SmoothTime);
		double Omega = 2.0F / SmoothTime;
		double DeltaOmega = Omega * DeltaTime;
		float Coefficient = 1.0F + DeltaOmega + (SMOOTH_DAMP_DELTA_SQUARED_COEFFICIENT * (DeltaOmega * DeltaOmega)) + (SMOOTH_DAMP_DELTA_CUBED_COEFFICIENT * ((DeltaOmega * DeltaOmega) * DeltaOmega)); // 1차, 2차, 3차 항의 합을 계산함으로써 얻는 값
		double Exp = 1.0F / Coefficient;
		T Change = Current - Target;
		T OriginalTarget = Target;
		double MaxChange = MaxSpeed * SmoothTime;
		Change = FMath::Clamp(Change, -MaxChange, MaxChange);
		Target = Current - Change;
		T DeltaChange = (CurrentVelocity + Omega * Change) * DeltaTime;
		CurrentVelocity = (CurrentVelocity - Omega * DeltaChange) * Exp;
		T Result = Target + (Change + DeltaChange) * Exp;
		if ((OriginalTarget - Current > 0.0F) == (Result > OriginalTarget))
		{
			Result = OriginalTarget;
			CurrentVelocity = DeltaTime > UE_KINDA_SMALL_NUMBER ? (Result - OriginalTarget) / DeltaTime : 0.0F;
		}
		return Result;
	}
};
