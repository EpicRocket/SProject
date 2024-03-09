
#include "UIHelper.h"
// include Engine
#include "Engine/GameInstance.h"
#include "CommonActivatableWidget.h"
#include "NativeGameplayTags.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
// include Plugin
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameUIManagerSubsystem.h"
#include "GameUIPolicy.h"
#include "CommonLocalPlayer.h"
#include "PrimaryGameLayout.h"
// include Project

#include UE_INLINE_GENERATED_CPP_BY_NAME(UIHelper)

FText UUIHelper::FormatWithSuffix(int64 Value, int32 FractionalDigits)
{
	static const FString Suffixes[] = { TEXT(""), TEXT("K"), TEXT("M"), TEXT("G"), TEXT("T"), TEXT("P") };
	static const int32 NumSuffixes = sizeof(Suffixes) / sizeof(*Suffixes);

	static FNumberFormattingOptions NumberFormattingOptions =
		FNumberFormattingOptions()
		.SetAlwaysSign(false)
		.SetUseGrouping(true)
		.SetRoundingMode(ERoundingMode::HalfToEven)
		.SetMinimumIntegralDigits(0)
		.SetMaximumIntegralDigits(TNumericLimits<int32>::Max())
		.SetMaximumFractionalDigits(TNumericLimits<int32>::Max());

	int32 SuffixIndex = 0;
	double DoubleValue = FMath::Abs(static_cast<double>(Value));

	while (DoubleValue >= 1000.0 && SuffixIndex < NumSuffixes - 1)
	{
		DoubleValue /= 1000.0;
		++SuffixIndex;
	}

	if (Value < 0)
	{
		DoubleValue *= 1.0;
	}

	if (FMath::Abs(DoubleValue) >= 100.0)
	{
		NumberFormattingOptions.SetMinimumFractionalDigits(FMath::Max(0, FractionalDigits - 2));
	}
	else if (FMath::Abs(DoubleValue) >= 10.0)
	{
		NumberFormattingOptions.SetMinimumFractionalDigits(FMath::Max(0, FractionalDigits - 1));
	}
	else
	{
		NumberFormattingOptions.SetMinimumFractionalDigits(FMath::Max(0, FractionalDigits));
	}

	return FText::Format(FTextFormat::FromString(TEXT("{0}{1}")), FText::AsNumber(DoubleValue, &NumberFormattingOptions), FText::FromString(Suffixes[SuffixIndex]));
}
