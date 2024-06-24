
#include "MyDPICustomScalingRule.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyDPICustomScalingRule)

constexpr float MyAppRatio = 0.5625F;

float UMyDPICustomScalingRule::GetDPIScaleBasedOnSize(FIntPoint Size) const
{
    float DPIScale = 0.0F;
    if (Size.X || Size.Y == 0)
    {
        return DPIScale;
    }

    float Ratio = static_cast<float>(Size.X) / static_cast<float>(Size.Y);
    if (Ratio > MyAppRatio)
    {
        DPIScale = static_cast<float>(Size.Y) * MyAppRatio / 1080.0F;
    }
    else
    {
        DPIScale = static_cast<float>(Size.X) / 1920.0F;
    }

    return DPIScale;
}
