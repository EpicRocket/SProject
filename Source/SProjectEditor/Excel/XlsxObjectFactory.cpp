
#include "XlsxObjectFactory.h"
#include "Misc/MessageDialog.h"
#include "ObjectTools.h"
#include "XlsxObject.h"
#include "HAL/FileManager.h"
#include "DataTableEditorUtils.h"
#include "Engine/UserDefinedEnum.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/UserDefinedStruct.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"
#include "Kismet2/StructureEditorUtils.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "Kismet2/EnumEditorUtils.h"

#if PLATFORM_WINDOWS
#include <Windows.h>
#include <WinNls.h>
#include <string>
#include <exception>
#include <set>
//! OpenXLSX Library
#include "OpenXLSX/OpenXLSX.hpp"
#include "OpenXLSX/headers/XLCellValue.hpp"
#endif

static std::string WStringToString(const std::wstring& wstr)
{
	int size = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[size];
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, chRtn, size, NULL, NULL);
	std::string str(chRtn);
	delete[] chRtn;
	return str;
}

static FString UTF8ToTCHARString(const std::string& utf8Str)
{
	if (utf8Str.empty())
	{
		return FString();
	}

	int32_t targetLength = MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, nullptr, 0);
	if (targetLength == 0)
	{
		return FString();
	}

	TArray<TCHAR> targetArray;
	targetArray.Reserve(targetLength);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, targetArray.GetData(), targetLength);

	return FString(targetArray.GetData());
}

static FString ToTypeString(const OpenXLSX::XLCellValueProxy& Proxy)
{
	switch (Proxy.type())
	{
	case OpenXLSX::XLValueType::Boolean: Proxy.get<bool>() ? TEXT("true") : TEXT("false");

	case OpenXLSX::XLValueType::Integer: return FString::Printf(TEXT("%d"), Proxy.get<int>());

	case OpenXLSX::XLValueType::Float: return FString::Printf(TEXT("%f"), Proxy.get<float>());

	case OpenXLSX::XLValueType::String: return UTF8ToTCHARString(Proxy.get<std::string>());
	}
	return FString();
}

enum class EAssetType : uint8
{
	None,
	Struct,
	Enum,
};

struct FXlsxObjectHeaderData
{
	FString Header;
	FString Type;

	FXlsxObjectHeaderData() {};
	FXlsxObjectHeaderData(FString InHeader, FString InType)
		: Header(InHeader), Type(InType)
	{}
};

struct FSheetData
{
	EAssetType AssetType = EAssetType::None;
	FString Name;
	TArray<FXlsxObjectHeaderData> Headers;
	TArray<TArray<FString>> Cells;
};


UXlsxObjectFactory::UXlsxObjectFactory()
{
	SupportedClass = UXlsxObject::StaticClass();
	Formats.Add(TEXT("xlsx;Microsoft Excel Spreadsheet"));
	bEditorImport = true;
	bText = false;
}

UObject* UXlsxObjectFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UXlsxObject* ExistingAsset = Cast<UXlsxObject>(StaticFindObject(nullptr, InParent, *InName.ToString()));
	if (ExistingAsset)
	{
		IFileManager& FileManager = IFileManager::Get();
		if (!FileManager.Delete(*ExistingAsset->SubsystemPath, false, false, true))
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("Could not delete existing asset %s"), *Filename)));
			return nullptr;
		}

		// Delete DataTables
		TArray<UObject*> DeleteTables;
		DeleteTables.Reserve(ExistingAsset->DataTables.Num());
		for (UDataTable* Item : ExistingAsset->DataTables)
		{
			DeleteTables.Emplace(Item);
		}
		ExistingAsset->DataTables.Empty();

		// Delete Structs
		TArray<UObject*> DeleteStructs;
		DeleteStructs.Reserve(ExistingAsset->Structs.Num());
		for (UObject* Item : ExistingAsset->Structs)
		{
			DeleteStructs.Emplace(Item);
		}
		ExistingAsset->Structs.Empty();

		// Delete Enums
		TArray<UObject*> DeleteEnums;
		DeleteEnums.Reserve(ExistingAsset->Enums.Num());
		for (UObject* Item : ExistingAsset->Enums)
		{
			DeleteEnums.Emplace(Item);
		}
		ExistingAsset->Enums.Empty();

		// Delete all objects
		TArray<UObject*> DeleteObjects;
		DeleteObjects.Reserve(DeleteTables.Num() + DeleteStructs.Num() + DeleteEnums.Num() + 1);
		DeleteObjects.Emplace(ExistingAsset);
		DeleteObjects.Append(DeleteTables);
		DeleteObjects.Append(DeleteStructs);
		DeleteObjects.Append(DeleteEnums);

		if (!ObjectTools::ForceDeleteObjects(DeleteObjects))
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("Could not delete existing asset %s"), *Filename)));
			return nullptr;
		}

		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	}

	TArray<FSheetData> SheetDatas;

	try
	{
		OpenXLSX::XLDocument Document(WStringToString(*Filename));
		if (!Document.isOpen())
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("Failed to open the document. [%s]"), *Filename)));
			return nullptr;
		}

		OpenXLSX::XLWorkbook WorkBook = Document.workbook();

		for (std::string const& Name : WorkBook.sheetNames())
		{
			FSheetData SheetData;
			SheetData.Name = FString(Name.c_str());

			if (SheetData.Name.Left(1) == TEXT("!"))
			{	// Blueprint Structure Type
				SheetData.AssetType = EAssetType::Struct;
			}
			else if (SheetData.Name.Left(1) == TEXT("@"))
			{	// Blueprint Enum Type
				SheetData.AssetType = EAssetType::Enum;
			}
			else
			{	// Unknown
				continue;
			}

			SheetData.Name = SheetData.Name.RightChop(1);

			try
			{
				OpenXLSX::XLWorksheet WorkSheet = WorkBook.worksheet(Name);
				int32 RowCount = WorkSheet.rowCount();
				if (RowCount < 2)
				{
					continue;
				}

				OpenXLSX::XLRow HeaderRow = WorkSheet.row(1);
				OpenXLSX::XLRow TypeRow = WorkSheet.row(2);
				auto HeaderRowIter = HeaderRow.cells().begin();
				auto TypeRowIter = TypeRow.cells().begin();

				for (; HeaderRowIter != HeaderRow.cells().end(); ++HeaderRowIter, ++TypeRowIter)
				{
					auto& Header = HeaderRowIter->value();
					auto& Type = TypeRowIter->value();

					FString HeaderName(Header.get<std::string>().c_str());
					FString TypeName(Type.get<std::string>().c_str());

					SheetData.Headers.Emplace(FXlsxObjectHeaderData(HeaderName, TypeName));
				}

				for (int32 i = 3; i < RowCount; ++i)
				{
					OpenXLSX::XLRow DataRow = WorkSheet.row(i);
					TArray<FString> Cells;
					for (auto Iter = DataRow.cells().begin(); Iter != DataRow.cells().end(); ++Iter)
					{
						auto& RowData = Iter->value();
						Cells.Emplace(ToTypeString(RowData));
					}
					SheetData.Cells.Emplace(Cells);
				}
			}
			catch (std::exception Exception)
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to open the sheet. [%s]"), *SheetData.Name);
				continue;
			}

			SheetDatas.Emplace(SheetData);
		}
	}
	catch (std::exception Exception)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to open the document. [%s]"), *UTF8ToTCHARString(Exception.what()))
		return nullptr;
	}

	SheetDatas.Sort([](FSheetData const& Left, FSheetData const& Right)
	{
		if (Left.AssetType == EAssetType::Enum && Right.AssetType == EAssetType::Struct)
		{
			return true;
		}
		else if (Left.AssetType == EAssetType::Struct && Right.AssetType == EAssetType::Enum)
		{
			return false;
		}
		else
		{
			return false;
		}
	});

	UXlsxObject* Result = NewObject<UXlsxObject>(InParent, InClass, FName(*FString::Printf(TEXT("XLSX_%s"), *InName.ToString())), Flags);

	for (auto const& SheetData : SheetDatas)
	{
		switch (SheetData.AssetType)
		{
		case EAssetType::Struct:
		{
			FString UserDefinedStructName = FString::Printf(TEXT("BS_%s"), *SheetData.Name);
			UUserDefinedStruct* NewUserDefinedStruct = NewObject<UUserDefinedStruct>(InParent, UUserDefinedStruct::StaticClass(), *UserDefinedStructName, Flags);

			NewUserDefinedStruct->EditorData = NewObject<UUserDefinedStructEditorData>(NewUserDefinedStruct, NAME_None, RF_Transactional);
			NewUserDefinedStruct->Guid = FGuid::NewGuid();
			NewUserDefinedStruct->SetMetaData(TEXT("BlueprintType"), TEXT("true"));
			NewUserDefinedStruct->Bind();
			NewUserDefinedStruct->StaticLink(true);
			NewUserDefinedStruct->Status = EUserDefinedStructureStatus::UDSS_Error;

			for (auto const& Header : SheetData.Headers)
			{
				FStructVariableDescription NewVar;
				NewVar.VarName = *Header.Header;
				NewVar.FriendlyName = *Header.Header;
				NewVar.SetPinType(FEdGraphPinType(FName(*Header.Type), NAME_None, nullptr, EPinContainerType::None, false, FEdGraphTerminalType()));
				NewVar.VarGuid = FGuid::NewGuid();
				FStructureEditorUtils::GetVarDesc(NewUserDefinedStruct).Add(NewVar);

				FStructureEditorUtils::OnStructureChanged(NewUserDefinedStruct, FStructureEditorUtils::EStructureEditorChangeInfo::AddedVariable);
			}

			FString DataTableName = FString::Printf(TEXT("DT_%s"), *SheetData.Name);
			UDataTable* NewDataTable = NewObject<UDataTable>(InParent, UDataTable::StaticClass(), *DataTableName, Flags);
			NewDataTable->RowStruct = NewUserDefinedStruct;

			// TODO: 저기 FOR문에서 루프 돌면 안됨
			for (auto const& Cell : SheetData.Cells)
			{
				FName RowName = FName(*Cell[0]);
				uint8* RowPtr = FDataTableEditorUtils::AddRow(NewDataTable, RowName);
				int32 RowIndex = 0;
				for (TFieldIterator<FProperty> It(NewDataTable->RowStruct); It; ++It) // <<<< FIX
				{
					FProperty* BaseProp = *It;
					check(BaseProp);
					if (Cell.IsValidIndex(RowIndex))
					{
						DataTableUtils::AssignStringToProperty(Cell[RowIndex++], BaseProp, RowPtr);
					}
					else
					{
						FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(FString::Printf(TEXT("[%s] is index invalid."), *SheetData.Name)));
						return nullptr;
					}
				}
			}

			Result->Structs.Emplace(NewUserDefinedStruct);
			Result->DataTables.Emplace(NewDataTable);
		} break;

		case EAssetType::Enum:
		{
			FString UserDefinedEnumName = FString::Printf(TEXT("BE_%s"), *SheetData.Name);
			UUserDefinedEnum* NewUserDefinedEnum = NewObject<UUserDefinedEnum>(InParent, UUserDefinedEnum::StaticClass(), *UserDefinedEnumName, Flags);

			NewUserDefinedEnum->SetMetaData(TEXT("BlueprintType"), TEXT("true"));
			NewUserDefinedEnum->Bind();

			TArray<TPair<FName, int64>> EnumValues;
			NewUserDefinedEnum->SetEnums(EnumValues, UEnum::ECppForm::Namespaced);

			for (auto const& Cell : SheetData.Cells)
			{
				EnumValues.Emplace(TPair<FName, int64>(FName(*Cell[0]), EnumValues.Num()));
				FEnumEditorUtils::AddNewEnumeratorForUserDefinedEnum(NewUserDefinedEnum);
				FEnumEditorUtils::SetEnumeratorDisplayName(NewUserDefinedEnum, EnumValues.Num() - 1, FText::FromString(EnumValues.Last().Key.ToString()));
			}

			Result->Enums.Emplace(NewUserDefinedEnum);
		} break;

		default: continue;
		}
	}

	for (auto const& Item : Result->Structs)
	{
		FAssetRegistryModule::AssetCreated(Item);
		GEditor->BroadcastObjectReimported(Item);
	}

	for (auto const& Item : Result->Enums)
	{
		FAssetRegistryModule::AssetCreated(Item);
		GEditor->BroadcastObjectReimported(Item);
	}

	for (auto const& Item : Result->DataTables)
	{
		FAssetRegistryModule::AssetCreated(Item);
		GEditor->BroadcastObjectReimported(Item);
	}

	return Result;
}
