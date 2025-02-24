
#pragma once

#include "CoreMinimal.h"

namespace OpenXLSX {
	class XLCell;
}

enum class EOpenXLSXType : uint8;

class OPENXLSX_API XLCell {
public:

	EOpenXLSXType Type() const;

	FString StringValue() const;

	int64 IntValue() const;

	double RealValue() const;

	bool BoolValue() const;

#if PLATFORM_WINDOWS
public:
	void SetCell(OpenXLSX::XLCell InCell);
private:
	TSharedPtr<OpenXLSX::XLCell> Cell;
#endif
};
