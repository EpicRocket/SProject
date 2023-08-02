

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "CinemachineInputAxis.generated.h"

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = true))
enum class ECVRestrictionFlag
{
	None = 0x00,
	RangeIsDriven = 0x01,
	NoRecentering = 0x02,
	Momentary = 0x04,
};

USTRUCT(BlueprintType)
struct FCVRecenteringSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "활성화", Tooltip = "설정하면 축의 자동 중심 재조정을 활성화합니다."))
	bool bEnabled = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bEnabled, Tooltip = "이 시간 동안 축에서 사용자 입력이 감지되지 않으면 다시 중심을 잡기 시작합니다.", ClampMin = 0.0, Units = "s"))
	float Wait = 1.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (EditCondition = bEnabled, Tooltip = "다시 중심을 잡기 시작한 후 중앙에 도달하는 데 걸리는 시간입니다.", ClampMin = 0.0, Units = "s"))
	float Time = 2.0F;
};

struct FCVRecenteringState
{
	float RecenteringVelocity = 0.0F;

	bool bForceRecenter = false;

	float LastValueChangeTime = 0.0F;

	float LastValue = 0.0F;
};

USTRUCT(BlueprintType)
struct FCinemachineInputAxis
{
	GENERATED_BODY()

public:
	FCinemachineInputAxis() = default;
	FCinemachineInputAxis(float InValue, FVector2D InRange, bool bInWarp, float InCenter)
		: Value(InValue)
		, Center(InCenter)
		, Range(InRange)
		, bWrap(bInWarp)
	{
	}

	void Validate()
	{
		Range.Y = FMath::Max(Range.X, Range.Y);
		Center = GetClampValue(Center);
		Value = GetClampValue(Value);
	}

	float GetClampValue(float InValue) const
	{
		const float D = Range.Y - Range.X;
		float Result = FMath::IsNearlyZero(D) ? 0.0F : FMath::Fmod(InValue - Range.X, D);
		if (Result < 0.0F)
		{
			Result += D;
		}
		Result += Range.X;
		Result = (bWrap && D > UE_KINDA_SMALL_NUMBER) ? Result : InValue;
		return FMath::Clamp(Result, Range.X, Range.Y);
	}

	float GetNormalizedValue() const
	{
		const float ClampValue = GetClampedValue();
		float D = Range.Y - Range.X;
		return (ClampValue - Range.X) / (D > UE_KINDA_SMALL_NUMBER ? D : 1.0F);
	}

	float GetClampedValue() const
	{
		return GetClampValue(Value);
	}

	void Reset(UWorld* World);

	bool TrackValueChange(UWorld* World);

	void SetValueAndLastValue(float InValue)
	{
		Value = RecenteringState.LastValue = InValue;
	}

	void UpdateRecentering(UWorld* World, float DeltaTime, bool bForceCancel);

	void TriggerRecentering()
	{
		RecenteringState.bForceRecenter = true;
	}

	void CancelRecentering(UWorld* World);

	void ProcessInput(float InputValue, float DeltaTime);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag InputAxisTag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "축의 값", Tooltip = "축의 현재 값입니다."))
	float Value = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "축 중심 값", Tooltip = "이 축의 중심 혹은 휴식 상태 값입니다."))
	float Center = 0.0F;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "유효 범위", Tooltip = "축 값의 유효한 범위입니다. 값이 이 범위에 고정됩니다."))
	FVector2D Range = FVector2D::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "설정하면 축이 최소/최대 값으로 감싸면서 루프를 형성합니다."))
	bool bWrap = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "업데이트 설정"))
	FCVRecenteringSettings Recentering;

	UPROPERTY(BlueprintReadWrite, meta = (Bitmask, BitmaskEnum = "/Script/Ccg.ECVRestrictionFlag"))
	int32 Restriction = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "가속도", Tooltip = "제공된 축을 최대 값으로 설정한 상태에서 최대 속도까지 가속하는 데 걸리는 시간(초)", ClampMin = 0.0))
	float AccelTime = 0.2F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (DisplayName = "감속도", Tooltip = "제공된 축이 중립 위치에 있는 경우 축을 0으로 감속하는 데 걸리는 시간(초)", ClampMin = 0.0))
	float DecelTime = 0.2F;

private:
	FCVRecenteringState RecenteringState;

	float CurrentSpeed = 0.0F;
};
