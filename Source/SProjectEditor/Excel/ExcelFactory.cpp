
#include "ExcelFactory.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/UserDefinedStruct.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"
#include "Kismet2/StructureEditorUtils.h"
#include "Kismet2/BlueprintEditorUtils.h"

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

    return NewUserDefinedStruct;
}

//FStructureEditorUtils::AddVariable(NewUserDefinedStruct, FEdGraphPinType(UEdGraphSchema_K2::PC_Boolean, NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType()));

// Open the Excel file using xlnt
	/*xlnt::workbook WorkBook;
	try
	{
		WorkBook.load(*Filename);
	}
	catch (std::exception const& Exception)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load Excel file: %s"), Exception.what());
		return nullptr;
	}

	for (int32 Index = 0; Index < WorkBook.sheet_count(); ++Index)
	{
		auto const& Sheet = WorkBook.sheet_by_index(Index);

		FString StructName = FString::Printf(TEXT("%hs"), Sheet.title().c_str());
		
	}*/