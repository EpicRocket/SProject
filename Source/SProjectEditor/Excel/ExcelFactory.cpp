
#include "ExcelFactory.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/UserDefinedStruct.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"
#include "Kismet2/StructureEditorUtils.h"
#include "Kismet2/BlueprintEditorUtils.h"

#if PLATFORM_WINDOWS
#include "OpenXLSX/OpenXLSX.hpp"
#include <Windows.h>
#include <WinNls.h>
#include <string>
#include <exception>
namespace
{
std::string WStringToString(const std::wstring& wstr)
{
	int size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[size];
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, chRtn, size, NULL, NULL);
	std::string str(chRtn);
	delete[] chRtn;
	return str;
}
}
#endif

FName UExcelFactory::StructureNameDef = "Structure";
FName UExcelFactory::EnumNameDef = "Enum";

UExcelFactory::UExcelFactory()
{
	SupportedClass = UObject::StaticClass();
    Formats.Add(TEXT("xlsx;Microsoft Excel Spreadsheet"));
	bEditorImport = true;
	bText = false;
}

UObject* UExcelFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	if (!FStructureEditorUtils::UserDefinedStructEnabled())
	{
		return nullptr;
	}
	
	// Get Filename path
	FString FilePath = FPaths::GetPath(Filename);


#if PLATFORM_WINDOWS
	try
	{
		// Open the workbook
		OpenXLSX::XLDocument Document(WStringToString(*Filename));

		if (!Document.isOpen())
		{
			return nullptr;
		}

		OpenXLSX::XLWorkbook WorkBook = Document.workbook();
		
		for (std::string const& Name : WorkBook.sheetNames())
		{
			FString SheetName = FString(Name.c_str());

			if (SheetName.Equals(StructureNameDef.ToString()))
			{
				// Create Structure
				try
				{
					// Open the worksheet
					OpenXLSX::XLWorksheet WorkSheet = WorkBook.worksheet(Name);
					
					int32 RowCount = WorkSheet.rowCount();
					if (RowCount < 2)
					{
						continue;
					}

					OpenXLSX::XLRow HeaderRow = WorkSheet.row(1);
					OpenXLSX::XLRow TypeRow = WorkSheet.row(2);

					
				}
				catch (std::exception e)
				{
					UE_LOG(LogTemp, Error, TEXT("OpenXLSX::XLWorksheet WorkSheet = WorkBook.worksheet(%s) Exception: %s"), *SheetName, *FString(e.what()));
					continue;
				}


			}
			else if (SheetName.Equals(EnumNameDef.ToString()))
			{
				
			}
			else
			{
				continue;
			}
		}

		
	}
	catch (std::exception e)
	{
		UE_LOG(LogTemp, Error, TEXT("OpenXLSX Error: %s"), *FString(e.what()));
		return nullptr;
	}
#endif

    return nullptr;
}

/*
FString UserDefinedStructName = FString::Printf(TEXT("BP_%sStructure"), *FPaths::GetBaseFilename(Filename));

	UUserDefinedStruct* NewUserDefinedStruct = Cast<UUserDefinedStruct>(CreateOrOverwriteAsset(UUserDefinedStruct::StaticClass(), InParent, *UserDefinedStructName, Flags));

	if(!NewUserDefinedStruct)
	{
		return nullptr;
	}

	NewUserDefinedStruct->EditorData = NewObject<UUserDefinedStructEditorData>(NewUserDefinedStruct, NAME_None, RF_Transactional);
	if (nullptr == NewUserDefinedStruct->EditorData)
	{
		return nullptr;
	}

	NewUserDefinedStruct->Guid = FGuid::NewGuid();
	NewUserDefinedStruct->SetMetaData(TEXT("BlueprintType"), TEXT("true"));
	NewUserDefinedStruct->Bind();
	NewUserDefinedStruct->StaticLink(true);
	NewUserDefinedStruct->Status = EUserDefinedStructureStatus::UDSS_Error;
*/





