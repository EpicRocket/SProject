
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

UExcelFactory::UExcelFactory()
{
	SupportedClass = UUserDefinedStruct::StaticClass();
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

	UUserDefinedStruct* NewUserDefinedStruct = Cast<UUserDefinedStruct>(CreateOrOverwriteAsset(UUserDefinedStruct::StaticClass(), InParent, InName, Flags));

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

#if PLATFORM_WINDOWS
	// Open the Excel file using OpenXLSX
	OpenXLSX::XLDocument Document;
	
	Document.open(::WStringToString(*Filename));
	
	for (std::string const& Name : Document.workbook().worksheetNames())
	{
		UE_LOG(LogTemp, Warning, TEXT("Sheet Name: %s"), *FString(Name.c_str()));
	}

	// Close the Excel file
	Document.close();

#endif

    return NewUserDefinedStruct;
}
