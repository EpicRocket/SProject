
#include "XLSXTableImportFactory.h"
// include Engine
#include "Editor.h"
#include "DataTableEditorUtils.h"
#include "JsonObjectConverter.h"
#include "Engine/DataTable.h"
#include "Engine/UserDefinedEnum.h"
#include "Modules/ModuleManager.h"
#include "Misc/MessageDialog.h"
#include "HAL/FileManager.h"
#include "Misc/ConfigCacheIni.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "EditorFramework/AssetImportData.h"
#include "Serialization/Csv/CsvParser.h"
#include "Serialization/JsonWriter.h"
#include "Serialization/JsonSerializer.h"
// include Plugin
#include "OpenXLSX.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(XLSXTableImportFactory)

DEFINE_LOG_CATEGORY(LogXLSXTableImporter);

UXLSXTableImportFactory::UXLSXTableImportFactory()
{
	bCreateNew = false;
	bEditAfterNew = false;
	SupportedClass = UDataTable::StaticClass();

	bEditorImport = true;
	bText = true;

	--ImportPriority;

	Formats.Add(TEXT("xlsx;"));
}

FText UXLSXTableImportFactory::GetDisplayName() const
{
	return FText::FromString(TEXT("XLSX Table Import"));
}

bool UXLSXTableImportFactory::DoesSupportClass(UClass* Class)
{
	return Class == UDataTable::StaticClass();
}

bool UXLSXTableImportFactory::FactoryCanImport(const FString& Filename)
{
	const FString Extension = FPaths::GetExtension(Filename);
	if (Extension == TEXT("xlsx"))
	{
		return true;
	}
	return false;
}

IImportSettingsParser* UXLSXTableImportFactory::GetImportSettingsParser()
{
	return this;
}

void UXLSXTableImportFactory::CleanUp()
{
	Super::CleanUp();

	bImportAll = false;
}

void UXLSXTableImportFactory::ParseFromJson(TSharedRef<class FJsonObject> ImportSettingsJson)
{
	FJsonObjectConverter::JsonObjectToUStruct(ImportSettingsJson, FXLSXTableImportSettings::StaticStruct(), &AutomatedImportSettings, 0, 0);
}

UObject* UXLSXTableImportFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	FString FileExtension = FPaths::GetExtension(Filename);

	check(bText); // 생성자에서 설정되므로 바이너리로 로드를 지원할 필요가 없습니다.

	FString Data = ConvertXLSXToJsonString(Filename);
	if (Data.IsEmpty())
	{
		UE_LOG(LogXLSXTableImporter, Error, TEXT("Failed to load file '%s to string'"), *Filename);
		return nullptr;
	}

	ParseParms(Parms);
	const TCHAR* Ptr = *Data;

	return FactoryCreateText(InClass, InParent, InName, Flags, nullptr, *FileExtension, Ptr, Ptr + Data.Len(), Warn, bOutOperationCanceled);
}

UObject* UXLSXTableImportFactory::FactoryCreateText(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPreImport(this, InClass, InParent, InName, Type);

	bOutOperationCanceled = false;
	
	FString JsonString(Buffer, BufferEnd - Buffer);
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonString);
	TSharedPtr<FJsonObject> JsonObject;

	if (!FJsonSerializer::Deserialize(Reader, JsonObject) || !JsonObject.IsValid())
	{
		UE_LOG(LogXLSXTableImporter, Error, TEXT("Failed to deserialize JSON string"));
		return nullptr;
	}

	const TArray<TSharedPtr<FJsonValue>>* JsonArray;
	if (!JsonObject->TryGetArrayField(TEXT("Sheets"), JsonArray))
	{
		UE_LOG(LogXLSXTableImporter, Error, TEXT("Failed to get 'Sheets' field"));
		return nullptr;
	}

	TArray<FXLSXImportSheetData> Sheets;
	for (auto& Value : *JsonArray)
	{
		TSharedPtr<FJsonObject> SheetObject = Value->AsObject();
		if (!SheetObject.IsValid())
		{
			UE_LOG(LogXLSXTableImporter, Error, TEXT("Failed to get sheet object"));
			return nullptr;
		}

		FXLSXImportSheetData SheetData;
		if (!SheetObject->TryGetStringField(TEXT("SheetName"), SheetData.SheetName))
		{
			UE_LOG(LogXLSXTableImporter, Error, TEXT("Failed to get 'SheetName' field"));
			return nullptr;
		}

		const TArray<TSharedPtr<FJsonValue>>* RowsArray;
		if (!SheetObject->TryGetArrayField(TEXT("Rows"), RowsArray))
		{
			UE_LOG(LogXLSXTableImporter, Error, TEXT("Failed to get 'Rows' field"));
			return nullptr;
		}

		for (auto& RowValue : *RowsArray)
		{
			TSharedPtr<FJsonObject> RowObject = RowValue->AsObject();
			if (!RowObject.IsValid())
			{
				UE_LOG(LogXLSXTableImporter, Error, TEXT("Failed to get row object"));
				return nullptr;
			}

			TArray<FString> Row;
			for (int32 Index = 0; ; ++Index)
			{
				FString ColName = FString::Printf(TEXT("Col%d"), Index);
				FString ColValue;
				if (!RowObject->TryGetStringField(ColName, ColValue))
				{
					break;
				}
				Row.Emplace(ColValue);
			}
			SheetData.Rows.Emplace(Row);
		}
		Sheets.Emplace(SheetData);
	}

	FString PackageName = InParent->GetName();
	int32 LastSlashIndex = INDEX_NONE;
	if (PackageName.FindLastChar('/', LastSlashIndex)) {
		PackageName = PackageName.Left(LastSlashIndex + 1);
	}

	FString CurrentPath = FPaths::GetPath(CurrentFilename);

	TArray<UObject*> NewAssets;
	TArray<FString> Problems;

	for (auto& [SheetName, Rows] : Sheets)
	{
		FXLSXTableImportSettings ImportSettings = AutomatedImportSettings;
		UObject* NewAsset = nullptr;

		FString DataTableName = FString::Printf(TEXT("DT_%s"), *SheetName);
		UScriptStruct* RowStruct = FindObjectSafe<UScriptStruct>(FTopLevelAssetPath(TEXT("/Script/SProject"), FName(FString::Printf(TEXT("%sTableRow"), *SheetName))));
		if (nullptr == RowStruct)
		{
			UE_LOG(LogXLSXTableImporter, Error, TEXT("Failed to find row struct. [Name:%s]"), *SheetName);
			continue;
		}
		UPackage* Pkg = CreatePackage(*(PackageName / DataTableName));

		UClass* DataTableClass = UDataTable::StaticClass();
		UDataTable* NewTable = NewObject<UDataTable>(InParent, DataTableClass, FName(*DataTableName), Flags);
		NewTable->RowStruct = RowStruct;

		if (RowStruct)
		{
			NewTable->RowStructName_DEPRECATED = RowStruct->GetFName();
			NewTable->RowStructPathName = RowStruct->GetStructPathName();
		}

		NewTable->AssetImportData->Update(CurrentPath / DataTableName);

		ImportSettings.ImportRowStruct = RowStruct;
		for (int32 RowIndex = 0; RowIndex < Rows.Num(); ++RowIndex)
		{
			bool bFirstCell = true;
			for (int32 ColIndex = 0; ColIndex < Rows[RowIndex].Num(); ++ColIndex)
			{
				if (!bFirstCell)
				{
					ImportSettings.DataToImport += TEXT(",");
				}
				else
				{
					bFirstCell = false;
				}
				ImportSettings.DataToImport += Rows[RowIndex][ColIndex];
			}
			if (RowIndex != Rows.Num() - 1)
			{
				ImportSettings.DataToImport += TEXT("\n");
			}
		}

		Problems.Append(DoImportDataTable(ImportSettings, NewTable));

		NewAsset = NewTable;

		UE_LOG(LogXLSXTableImporter, Log, TEXT("Import DataTable '%s' - %d Problems"), *SheetName, Problems.Num());

		if (NewAsset)
		{
			NewAssets.Emplace(NewAsset);
		}
	}

	if (Problems.Num() > 0)
	{
		FString AllProblems;

		for (int32 ProbIdx = 0; ProbIdx < Problems.Num(); ProbIdx++)
		{
			UE_LOG(LogXLSXTableImporter, Log, TEXT("%d:%s"), ProbIdx, *Problems[ProbIdx]);
			AllProblems += Problems[ProbIdx];
			AllProblems += TEXT("\n");
		}

		if (!IsAutomatedImport())
		{
			FMessageDialog::Open(EAppMsgType::Ok, FText::FromString(AllProblems));
		}
	}

	for (auto& NewAsset : NewAssets)
	{
		if (NewAsset)
		{
			GEditor->GetEditorSubsystem<UImportSubsystem>()->BroadcastAssetPostImport(this, NewAsset);
		}
	}

	return NewAssets.IsEmpty() ? nullptr : NewAssets[0];
}

EReimportResult::Type UXLSXTableImportFactory::ReimportCSV(UObject* Obj)
{
	EReimportResult::Type Result = EReimportResult::Failed;

	if (UDataTable* DataTable = Cast<UDataTable>(Obj))
	{
		FDataTableEditorUtils::BroadcastPreChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
		Result = Reimport(DataTable, DataTable->AssetImportData->GetFirstFilename());
		FDataTableEditorUtils::BroadcastPostChange(DataTable, FDataTableEditorUtils::EDataTableChangeInfo::RowList);
	}

	return Result;
}

EReimportResult::Type UXLSXTableImportFactory::Reimport(UObject* Obj, const FString& Path)
{
	if (Path.IsEmpty())
	{
		return EReimportResult::Failed;
	}

	FString FilePath = IFileManager::Get().ConvertToRelativePath(*Path);

	FString Data = ConvertXLSXToJsonString(Path);
	const TCHAR* Ptr = *Data;
	CurrentFilename = FilePath;
	bool bWasCancelled = false;
	UObject* Result = FactoryCreateText(Obj->GetClass(), Obj->GetOuter(), Obj->GetFName(), Obj->GetFlags(), nullptr, *FPaths::GetExtension(FilePath), Ptr, Ptr + Data.Len(), nullptr, bWasCancelled);
	if (bWasCancelled)
	{
		return EReimportResult::Cancelled;
	}

	return Result ? EReimportResult::Succeeded : EReimportResult::Failed;
}

TArray<FString> UXLSXTableImportFactory::DoImportDataTable(const FXLSXTableImportSettings& ImportSettings, class UDataTable* TargetDataTable)
{
	return TargetDataTable->CreateTableFromCSVString(ImportSettings.DataToImport);
}

FString UXLSXTableImportFactory::ConvertXLSXToJsonString(const FString& Filename)
{
	FString Result;
	XLDocument Document;
	Document.Open(Filename);
	auto& Workbook = Document.WorkBook;
	if (Workbook->WorksheetCount() == 0)
	{
		UE_LOG(LogXLSXTableImporter, Error, TEXT("No worksheet found [FileName:%s]"), *Filename);
		return TEXT("");
	}
	/*
	{
	StructName
	StructType(DataTable, Const)
	---, Row1, Row2, Row3, ...
	Key, Col1, Col1, Col1, ...
	}
	*/
	TArray<FXLSXImportSheetData> SheetDatas;
	for (auto& [Name, Sheet] : Workbook->Worksheets)
	{
		FXLSXImportSheetData Data;
		FString TypeDelimiters = Name.Left(1);
		if (!TypeDelimiters.Equals(TEXT("!")))
		{
			continue;
		}
		Data.SheetName = Name.RightChop(1);

		for (auto& Row : Sheet->Rows())
		{
			TArray<FString> RowData;
			for (auto& Cell : Row->Cells())
			{
				switch (Cell->Type())
				{
				case EOpenXLSXType::String:
					RowData.Emplace(FString::Printf(TEXT("\"%s\""), *Cell->StringValue()));
					break;

				case EOpenXLSXType::Integer:
					RowData.Emplace(FString::Printf(TEXT("%lld"), Cell->IntValue()));
					break;

				case EOpenXLSXType::Float:
					RowData.Emplace(FString::Printf(TEXT("%f"), Cell->RealValue()));
					break;

				case EOpenXLSXType::Boolean:
					RowData.Emplace(FString::Printf(TEXT("%s"), Cell->BoolValue() ? TEXT("True") : TEXT("False")));
					break;

				default:
					RowData.Emplace(TEXT(""));
					break;
				}
			}
			Data.Rows.Emplace(RowData);
		}

		SheetDatas.Emplace(Data);
	}

	// 비어있는 열 제거
	for (auto& SheetData : SheetDatas)
	{
		if (SheetData.Rows.Num() <= 1)
		{
			// 잘못된 시트가 존재함
			UE_LOG(LogXLSXTableImporter, Error, TEXT("Invalid sheet data [SheetName:%s]"), *SheetData.SheetName);
			return TEXT("");
		}

		auto& TypeRow = SheetData.Rows[1];
		TArray<int32> DeleteRows;
		for (int32 Index = 0; Index < TypeRow.Num(); ++Index)
		{
			if (TypeRow[Index].IsEmpty())
			{
				DeleteRows.Emplace(Index);
			}
		}

		for (auto& Row : SheetData.Rows)
		{
			for (int32 Index : DeleteRows)
			{
				Row.RemoveAt(Index);
			}
		}
	}

	// 필요 없는 행 제거
	for (auto& SheetData : SheetDatas)
	{
		SheetData.Rows.RemoveAt(1);	// 타입
	}

	// Row 데이터 생성
	for (auto& SheetData : SheetDatas)
	{
		for (int32 Row = 0; Row < SheetData.Rows.Num(); ++Row)
		{
			auto& Cells = SheetData.Rows[Row];
			if (Row == 0)
			{
				Cells.Insert(TEXT("---"), 0);
			}
			else
			{
				FString Key = Cells[0];
				Cells.Insert(Key, 0);
			}
		}
	}
	
	// 시트 데이터 JsonObject로 변환
	TSharedRef<FJsonObject> JsonObject = MakeShareable(new FJsonObject());
	TArray<TSharedPtr<FJsonValue>> JsonArray;

	for (auto& SheetData : SheetDatas)
	{
		TSharedPtr<FJsonObject> SheetJsonObject = MakeShareable(new FJsonObject());
		SheetJsonObject->SetStringField(TEXT("SheetName"), SheetData.SheetName);
		TArray<TSharedPtr<FJsonValue>> RowsArray;
		for (auto& Row : SheetData.Rows)
		{
			TSharedPtr<FJsonObject> RowJsonObject = MakeShareable(new FJsonObject());
			for (int32 Index = 0; Index < Row.Num(); ++Index)
			{
				RowJsonObject->SetStringField(FString::Printf(TEXT("Col%d"), Index), Row[Index]);
			}
			RowsArray.Emplace(MakeShareable(new FJsonValueObject(RowJsonObject)));
		}
		SheetJsonObject->SetArrayField(TEXT("Rows"), RowsArray);
		JsonArray.Emplace(MakeShareable(new FJsonValueObject(SheetJsonObject)));
	}
	JsonObject->SetArrayField(TEXT("Sheets"), JsonArray);
	
	// JSON을 FString으로 직렬화
	TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&Result);
	FJsonSerializer::Serialize(JsonObject, Writer);

	return Result;
}
