
#pragma once

#include "CoreMinimal.h"

#if PLATFORM_WINDOWS
namespace OpenXLSX {
	class XLDocument;
}
#endif

class XLWorkbook;

class OPENXLSX_API XLDocument {
public:
	XLDocument();
	~XLDocument();

	void Open(FString FilePath);

	TSharedPtr<XLWorkbook> WorkBook;

#if PLATFORM_WINDOWS
private:
	TSharedPtr<OpenXLSX::XLDocument> Document;
#endif
};
