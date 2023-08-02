

#pragma once

#include "Shared/Rect.h"
#include "CinemachineScreenComposerSettings.generated.h"

USTRUCT(BlueprintType)
struct FCVDeadZoneSettings
{
	GENERATED_BODY()

public:
	FCVDeadZoneSettings() = default;
	FCVDeadZoneSettings(bool bInEnabled, float InSize)
		: bEnabled(bInEnabled)
		, X(InSize)
		, Y(InSize)
	{
	}
	FCVDeadZoneSettings(bool bInEnabled, float InX, float InY)
		: bEnabled(bInEnabled)
		, X(InX)
		, Y(InY)
	{
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 2.0, UIMin = 0.0, UIMax = 2.0))
	float X = 0.5F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 2.0, UIMin = 0.0, UIMax = 2.0))
	float Y = 0.5F;
};

USTRUCT(BlueprintType)
struct FCVHardLimitSettings
{
	GENERATED_BODY()

public:
	FCVHardLimitSettings() = default;
	FCVHardLimitSettings(bool bInEnabled, float InSize, float InOffset)
		: bEnabled(bInEnabled)
		, X(InSize)
		, Y(InSize)
		, OffsetX(InOffset)
		, OffsetY(InOffset)
	{
	}
	FCVHardLimitSettings(bool bInEnabled, float InX, float InY, float InOffsetX, float InOffsetY)
		: bEnabled(bInEnabled)
		, X(InX)
		, Y(InY)
		, OffsetX(InOffsetX)
		, OffsetY(InOffsetY)
	{
	}

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 3.0, UIMin = 0.0, UIMax = 3.0))
	float X = 0.5F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = 0.0, ClampMax = 3.0, UIMin = 0.0, UIMax = 3.0))
	float Y = 0.5F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -1.0, ClampMax = 1.0, UIMin = -1.0, UIMax = 1.0))
	float OffsetX = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -1.0, ClampMax = 1.0, UIMin = -1.0, UIMax = 1.0))
	float OffsetY = 0.0F;
};

USTRUCT(BlueprintType)
struct FCinemachineScreenComposerSettings
{
	GENERATED_BODY()

public:
	static FCinemachineScreenComposerSettings Lerp(const FCinemachineScreenComposerSettings& A, const FCinemachineScreenComposerSettings& B, float Alpha);

	static bool IsNearlyEqual(const FCinemachineScreenComposerSettings& A, const FCinemachineScreenComposerSettings& B, float Tolerance = UE_KINDA_SMALL_NUMBER);

public:
	FCinemachineScreenComposerSettings();
	FCinemachineScreenComposerSettings(float InScreenX, float InScreenY, bool bInDeadZoneEnable, float InDeadZoneX, float InDeadZoneY, bool bInHardLimitEnable, float InHardLimitX, float InHardLimitY, float InHardLmitOffsetX, float InHardLimitOffsetY);

	FVector2D GetEffectiveDeadZoneSize() const;

	FVector2D GetEffectiveHardLimitSize() const;

	FRect GetDeadZoneRect() const;

	void SetDeadZoneRect(FRect Rect);

	FRect GetHardLimitRect() const;

	void SetHardLimitRect(FRect Rect);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -1.5, ClampMax = 1.5, UIMin = -1.5, UIMax = 1.5, Tooltip = "대상의 화면 위치. 카메라가 추적 대상의 위치를 여기에 맞추도록 조정합니다. 0은 화면 중앙, +0.5 또는 -0.5는 화면 가장자리입니다."))
	float ScreenX = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin = -1.5, ClampMax = 1.5, UIMin = -1.5, UIMax = 1.5, Tooltip = "대상의 화면 위치. 카메라가 추적 대상의 위치를 여기에 맞추도록 조정합니다. 0은 화면 중앙, +0.5 또는 -0.5는 화면 가장자리입니다."))
	float ScreenY = 0.0F;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "타겟이 화면 위치의 이 범위 내에 있으면 카메라가 조정되지 않습니다."))
	FCVDeadZoneSettings DeadZone;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Tooltip = "타겟이 이 영역을 벗어나는 것은 허용되지 않습니다. 타겟이 이 영역 내에 있으면 감쇠 속도에 따라 카메라가 원하는 위치로 다시 정렬되도록 서서히 조정됩니다."))
	FCVHardLimitSettings HardLimits;
};
