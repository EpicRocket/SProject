

#include "CinemachineScreenComposerSettings.h"

FCinemachineScreenComposerSettings FCinemachineScreenComposerSettings::Lerp(const FCinemachineScreenComposerSettings& A, const FCinemachineScreenComposerSettings& B, float Alpha)
{
	return FCinemachineScreenComposerSettings(
		FMath::Lerp(A.ScreenX, B.ScreenX, Alpha),
		FMath::Lerp(A.ScreenY, B.ScreenY, Alpha),
		A.DeadZone.bEnabled || B.DeadZone.bEnabled,
		FMath::Lerp(A.DeadZone.X, B.DeadZone.X, Alpha),
		FMath::Lerp(A.DeadZone.Y, B.DeadZone.Y, Alpha),
		A.HardLimits.bEnabled || B.HardLimits.bEnabled,
		FMath::Lerp(A.HardLimits.X, B.HardLimits.X, Alpha),
		FMath::Lerp(A.HardLimits.Y, B.HardLimits.Y, Alpha),
		FMath::Lerp(A.HardLimits.OffsetX, B.HardLimits.OffsetX, Alpha),
		FMath::Lerp(A.HardLimits.OffsetY, B.HardLimits.OffsetY, Alpha)
	);
}

bool FCinemachineScreenComposerSettings::IsNearlyEqual(const FCinemachineScreenComposerSettings& A, const FCinemachineScreenComposerSettings& B, float Tolerance)
{
	return FMath::IsNearlyEqual(A.ScreenX, B.ScreenX)
		&& FMath::IsNearlyEqual(A.ScreenY, B.ScreenY)
		&& FMath::IsNearlyEqual(A.DeadZone.X, B.DeadZone.X)
		&& FMath::IsNearlyEqual(A.DeadZone.Y, B.DeadZone.Y)
		&& FMath::IsNearlyEqual(A.HardLimits.X, B.HardLimits.X)
		&& FMath::IsNearlyEqual(A.HardLimits.Y, B.HardLimits.Y)
		&& FMath::IsNearlyEqual(A.HardLimits.OffsetX, B.HardLimits.OffsetX)
		&& FMath::IsNearlyEqual(A.HardLimits.OffsetY, B.HardLimits.OffsetY);
}

FCinemachineScreenComposerSettings::FCinemachineScreenComposerSettings()
	: DeadZone(false, 2.0F)
	, HardLimits(false, 0.8F, 0.0F)
{
}

FCinemachineScreenComposerSettings::FCinemachineScreenComposerSettings(float InScreenX, float InScreenY, bool bInDeadZoneEnable, float InDeadZoneX, float InDeadZoneY, bool bInHardLimitEnable, float InHardLimitX, float InHardLimitY, float InHardLmitOffsetX, float InHardLimitOffsetY)
	: ScreenX(InScreenX)
	, ScreenY(InScreenY)
	, DeadZone(bInDeadZoneEnable, InDeadZoneX, InDeadZoneY)
	, HardLimits(bInHardLimitEnable, InHardLimitX, InHardLimitY, InHardLmitOffsetX, InHardLimitOffsetY)
{
}

FVector2D FCinemachineScreenComposerSettings::GetEffectiveDeadZoneSize() const
{
    return DeadZone.bEnabled ? FVector2D(DeadZone.X, DeadZone.Y) : FVector2D::ZeroVector;
}

FVector2D FCinemachineScreenComposerSettings::GetEffectiveHardLimitSize() const
{
    return HardLimits.bEnabled ? FVector2D(HardLimits.X, HardLimits.Y) : FVector2D(3.0F, 3.0F);
}

FRect FCinemachineScreenComposerSettings::GetDeadZoneRect() const
{
	const FVector2D DeadZoneSize = GetEffectiveDeadZoneSize();
	return FRect(FVector2D(ScreenX, ScreenY) - DeadZoneSize * 0.5F + FVector2D(0.5f, 0.5f), DeadZoneSize);
}

void FCinemachineScreenComposerSettings::SetDeadZoneRect(FRect Rect)
{
	FVector2D DeadZoneSize = GetEffectiveDeadZoneSize();
	if (DeadZone.bEnabled)
	{
		DeadZoneSize = FVector2D(FMath::Clamp(Rect.Width, 0.0F, 2.0F), FMath::Clamp(Rect.Height, 0.0F, 2.0F));
		DeadZone.X = DeadZoneSize.X;
		DeadZone.Y = DeadZoneSize.Y;
	}
	ScreenX = FMath::Clamp(Rect.X - 0.5F + DeadZoneSize.X * 0.5F, -1.5F, 1.5F);
	ScreenY = FMath::Clamp(Rect.Y - 0.5F + DeadZoneSize.Y * 0.5F, -1.5F, 1.5F);
	HardLimits.X = FMath::Clamp(HardLimits.X, DeadZoneSize.X, 3.0F);
	HardLimits.Y = FMath::Clamp(HardLimits.Y, DeadZoneSize.Y, 3.0F);
}

FRect FCinemachineScreenComposerSettings::GetHardLimitRect() const
{
	if (!HardLimits.bEnabled)
	{
		return FRect(-GetEffectiveHardLimitSize() * 0.5F, GetEffectiveHardLimitSize());
	}
	const FVector2D DeadZoneSize = GetEffectiveDeadZoneSize();
	const FVector2D HardLimitSize = FVector2D(HardLimits.X, HardLimits.Y);
	FRect Rect = FRect(FVector2D(ScreenX, ScreenY) - HardLimitSize * 0.5F + FVector2D(0.5F, 0.5F), HardLimitSize);
	Rect += FVector2D(HardLimits.OffsetX * 0.5F * (HardLimits.X - DeadZoneSize.X), HardLimits.OffsetY * 0.5F * (HardLimits.Y - DeadZoneSize.Y));
	return Rect;
}

void FCinemachineScreenComposerSettings::SetHardLimitRect(FRect Rect)
{
	HardLimits.X = FMath::Clamp(Rect.Width, 0.0F, 3.0F);
	HardLimits.Y = FMath::Clamp(Rect.Height, 0.0F, 3.0F);
	DeadZone.X = FMath::Min(DeadZone.X, HardLimits.X);
	DeadZone.Y = FMath::Min(DeadZone.Y, HardLimits.Y);
	// TODO: bias
}
