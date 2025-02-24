
#include "XLDocument.h"

#include "OpenXLSXLogging.h"
#include "OpenXLSXUtil.h"
#include "XLWorkbook.h"

#if PLATFORM_WINDOWS
#include "OpenXLSX/OpenXLSX.hpp"
#endif

XLDocument::XLDocument()
{
	WorkBook = MakeShared<XLWorkbook>();

#if PLATFORM_WINDOWS
	Document = MakeShared<OpenXLSX::XLDocument>();
#endif
}

XLDocument::~XLDocument()
{
#if PLATFORM_WINDOWS
	if (Document.IsValid())
	{
		Document->close();
	}
#endif
}

void XLDocument::Open(FString FilePath)
{
#if PLATFORM_WINDOWS
	try
	{
		Document->open(OpenXLSX::WStringToString(*FilePath));
		WorkBook->SetWorkbook(Document->workbook());
	}
	catch (std::exception& Exception)
	{
		UE_LOGFMT(LogOpenXLSX, Error, "{What}", *OpenXLSX::UTF8ToTCHARString(Exception.what()));
	}
#endif
}
