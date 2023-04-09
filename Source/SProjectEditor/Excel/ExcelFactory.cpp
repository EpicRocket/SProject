
#include "ExcelFactory.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/UserDefinedStruct.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"
#include "Kismet2/StructureEditorUtils.h"
#include "Kismet2/BlueprintEditorUtils.h"

#if PLATFORM_WINDOWS
#include "OpenXLSX/OpenXLSX.hpp"
#include "OpenXLSX/headers/XLCellValue.hpp"
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
	TArray<UObject*> AssetsToSave;
#if PLATFORM_WINDOWS
	try
	{
		// Open the workbook
		OpenXLSX::XLDocument Document(::WStringToString(*Filename));

		if (!Document.isOpen())
		{
			return nullptr;
		}

		OpenXLSX::XLWorkbook WorkBook = Document.workbook();
		
		for (std::string const& Name : WorkBook.sheetNames())
		{
			FString SheetName = FString(Name.c_str());
			UExcelFactory::Type BlueprintType;
			
			if (SheetName.Left(1) == TEXT("!"))
			{	// Blueprint Structure Type
				BlueprintType = UExcelFactory::Type::Structure;
			}
			else if (SheetName.Left(1) == TEXT("@"))
			{	// Blueprint Enum Type
				BlueprintType = UExcelFactory::Type::Enum;
			}
			else
			{	// Unknown
				continue;
			}

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

				using TableStructure = TTuple<FString, FString>;
				TArray<TableStructure> TableStructures;

				auto HeaderRowIter = HeaderRow.cells().begin();
				auto TypeRowIter = TypeRow.cells().begin();

				for (; HeaderRowIter != HeaderRow.cells().end(); ++HeaderRowIter, ++TypeRowIter)
				{
					auto& Header = HeaderRowIter->value();
					auto& Type = TypeRowIter->value();

					FString HeaerName(Header.get<std::string>().c_str());
					FString TypeName(Type.get<std::string>().c_str());
					TableStructures.Add(TableStructure(HeaerName, TypeName));
				}

				if (TableStructures.Num() == 0)
				{
					continue;
				}

				FString DefaultName = SheetName.Right(SheetName.Len() - 1);

				switch (BlueprintType)
				{
				case UExcelFactory::Structure:
				{
					FString UserDefinedStructName = FString::Printf(TEXT("BS_%s"), *DefaultName);
					UUserDefinedStruct* NewUserDefinedStruct = Cast<UUserDefinedStruct>(CreateOrOverwriteAsset(UUserDefinedStruct::StaticClass(), InParent, *UserDefinedStructName, Flags));

					if (!NewUserDefinedStruct)
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

					for (auto& [Header, Type] : TableStructures)
					{
						FStructVariableDescription NewVar;
						NewVar.VarName = *Header;
						NewVar.FriendlyName = *Header;
						NewVar.SetPinType(FEdGraphPinType(FName(*Type), NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType()));
						NewVar.VarGuid = FGuid::NewGuid();
						FStructureEditorUtils::GetVarDesc(NewUserDefinedStruct).Add(NewVar);

						FStructureEditorUtils::OnStructureChanged(NewUserDefinedStruct, FStructureEditorUtils::EStructureEditorChangeInfo::AddedVariable);
					}

					AssetsToSave.Emplace(NewUserDefinedStruct);

					// Create DataTable Blueprint
					FString DataTableName = FString::Printf(TEXT("DT_%s"), *DefaultName);
					UDataTable* NewDataTable = Cast<UDataTable>(CreateOrOverwriteAsset(UDataTable::StaticClass(), InParent, *DataTableName, Flags));
					
					if (!NewDataTable)
					{
						return nullptr;
					}

					NewDataTable->RowStruct = NewUserDefinedStruct;

					// Add data values
					//for (int32 RowIndex = 3; RowIndex <= RowCount; ++RowIndex)
					//{
					//	OpenXLSX::XLRow DataRow = WorkSheet.row(RowIndex);
					//	auto CellData = DataRow.cells().begin();

					//	NewDataTable->CreateTableFromCSVString()

					//	for (auto& [Header, Type] : TableStructures)
					//	{
					//		if (Header == TEXT("Index"))
					//		{


					//		}
					//		else if (Type == TEXT("int"))
					//		{

					//		}

					//	}

					//	NewDataTable->AddRow()

					//	//for (auto& Cell : DataRow.cells())
					//	//{
					//	//
					//	//
					//	//}

					//	
					//	FName RowName = FName(FString::Printf(TEXT("Row_%d"), RowIndex - 2));
					//	NewDataTable->AddRow(RowName);

					//	for (int32 ColIndex = 1; ColIndex <= TableStructures.Num(); ++ColIndex)
					//	{
					//		FString CellValue = FString(WorkSheet.cell(RowIndex, ColIndex).get<std::string>().c_str());
					//		NewDataTable->SetCell(RowName, *TableStructures[ColIndex - 1].Get<0>(), CellValue);
					//	}
					//}
					
					AssetsToSave.Emplace(NewDataTable);
				}
					break;
				case UExcelFactory::Enum:
				{
					// Create Enum Blueprint
				}
					break;
				default: continue;
				}
			}
			catch (std::exception e)
			{
				UE_LOG(LogTemp, Error, TEXT("OpenXLSX::XLWorksheet WorkSheet = WorkBook.worksheet(%s) Exception: %s"), *SheetName, *FString(e.what()));
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

	for (int32 i = 1; i < AssetsToSave.Num(); ++i)
	{
		FAssetRegistryModule::AssetCreated(AssetsToSave[i]);
	}

    return AssetsToSave.Num() == 0 ? nullptr : AssetsToSave[0];
}
