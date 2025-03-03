
#pragma once

#include "CoreMinimal.h"

namespace OpenXLSX {
	class XLRow;
}

class XLCell;

class OPENXLSX_API XLRow {
public:
	TArray<TSharedPtr<XLCell>> Cells() const;

#if PLATFORM_WINDOWS
public:
	void SetRow(OpenXLSX::XLRow InRow);
private:
	TSharedPtr<OpenXLSX::XLRow> Row;
#endif
};
