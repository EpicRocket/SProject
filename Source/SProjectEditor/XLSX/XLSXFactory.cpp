
#include "XLSXFactory.h"
// include Engine
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
#include "ObjectTools.h"
#include "Misc/FileHelper.h"
#include "Misc/ConfigCacheIni.h"
#include "Factories/DataTableFactory.h"
#include "Containers/Ticker.h"
// include LiveCoding
#include "ILiveCodingModule.h"
// include Project
#include "Helper/SStringHelper.h"
// include api
#include <exception>
#include <ranges>
#include <variant>
#include <type_traits>
#if PLATFORM_WINDOWS
#include "OpenXLSX/OpenXLSX.hpp"
#include "OpenXLSX/headers/XLCellValue.hpp"
#endif

#define Dependency_Module TEXT("SProject")
#define Dependency_Module_API TEXT("SPROJECT_API")
#define Dependency_Module_Name TEXT("/Script/SProject")

namespace XLSX
{
template<typename T>
T GetXLSXValue(const OpenXLSX::XLCellValueProxy& Proxy)
{
	switch (Proxy.type())
	{
	case OpenXLSX::XLValueType::Boolean: {
		bool Value = Proxy.get<bool>();
		if constexpr (std::is_same_v<T, bool>)
		{
			return Value;
		}
		else if constexpr (std::is_same_v<T, int32>)
		{
			return static_cast<int32>(Value);
		}
		else if constexpr (std::is_same_v<T, int64>)
		{
			return static_cast<int64>(Value);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			return static_cast<float>(Value);
		}
		else if constexpr (std::is_same_v<T, FString>)
		{
			return Value ? TEXT("TRUE") : TEXT("FALSE");
		}
	}
									   break;

	case OpenXLSX::XLValueType::Integer:
	{
		int Value = Proxy.get<int>();
		if constexpr (std::is_same_v<T, bool>)
		{
			return static_cast<bool>(Value);
		}
		else if constexpr (std::is_same_v<T, int32>)
		{
			return static_cast<int32>(Value);
		}
		else if constexpr (std::is_same_v<T, int64>)
		{
			return static_cast<int64>(Value);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			return static_cast<float>(Value);
		}
		else if constexpr (std::is_same_v<T, FString>)
		{
			return FString::Printf(TEXT("%d"), static_cast<int32>(Value));
		}
	}
	break;

	case OpenXLSX::XLValueType::Float:
	{
		float Value = Proxy.get<float>();
		if constexpr (std::is_same_v<T, bool>)
		{
			return !FMath::IsNearlyZero(Value);
		}
		else if constexpr (std::is_same_v<T, int32>)
		{
			return static_cast<int32>(Value);
		}
		else if constexpr (std::is_same_v<T, int64>)
		{
			return static_cast<int64>(Value);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			return static_cast<float>(Value);
		}
		else if constexpr (std::is_same_v<T, FString>)
		{
			return FString::Printf(TEXT("%f"), Value);
		}
	}
	break;

	case OpenXLSX::XLValueType::String:
	{
		FString Value = Helper::UTF8ToTCHARString(Proxy.get<std::string>());
		if constexpr (std::is_same_v<T, bool>)
		{
			return Value.Equals(TEXT("TRUE"), ESearchCase::IgnoreCase);
		}
		else if constexpr (std::is_same_v<T, int32>)
		{
			return FCString::Atoi(*Value);
		}
		else if constexpr (std::is_same_v<T, int64>)
		{
			return FCString::Atoi64(*Value);
		}
		else if constexpr (std::is_same_v<T, float>)
		{
			return FCString::Atof(*Value);
		}
		else if constexpr (std::is_same_v<T, FString>)
		{
			return Value;
		}
	}
	break;
	}
	return T{};
}
} // XLSX

TArray<UXLSXFactory::XLSXSheet> CacheSheet;
FDelegateHandle CompileHandle;
FTSTicker::FDelegateHandle NextTickHandle;
UObject* CacheTableAsset;
FString PackageName;

//////////////////////////////////////////////////////////////////////////
// UXLSXFactory
//////////////////////////////////////////////////////////////////////////

UXLSXFactory::UXLSXFactory()
{
	SupportedClass = UTempTableAsset::StaticClass();
	Formats.Add(TEXT("xlsx;Microsoft Excel Spreadsheet"));
	bEditorImport = true;
	bText = false;
}

UObject* UXLSXFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	if (IsCompileOnImportEnabled())
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to import. [Reason:Compiling]"));
		return nullptr;
	}

	if (!GenerateXLSXSheet(Filename))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate sheet. [Path:%s]"), *Filename);
		return nullptr;
	}

	FString AbsPath = FPaths::ProjectDir() / TEXT("Source") / Dependency_Module / GetTablePath();
	if (!FPaths::DirectoryExists(AbsPath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find module path. [Path:%s]"), *AbsPath);
		return nullptr;
	}

	FString FilePath = AbsPath / FString::Printf(TEXT("%s.h"), *InName.ToString());

	FString PreviousDesc;
	FFileHelper::LoadFileToString(PreviousDesc, *FilePath);

	FString ChangeToDesc = GenerateTableDesc(InName.ToString());
	FFileHelper::SaveStringToFile(ChangeToDesc, *FilePath);

	bool Unchanged = PreviousDesc.Equals(ChangeToDesc);

	PackageName = InParent->GetName();
	int32 LastSlashIndex = INDEX_NONE;
	if (PackageName.FindLastChar('/', LastSlashIndex)) {
		PackageName = PackageName.Left(LastSlashIndex + 1);
	}

	if (!Unchanged)
	{
		ILiveCodingModule* LiveCoding = FModuleManager::GetModulePtr<ILiveCodingModule>(LIVE_CODING_MODULE_NAME);
		CompileHandle.Reset();
		CompileHandle = LiveCoding->GetOnPatchCompleteDelegate().AddLambda(
			[]()
		{
			OnComplete();
		});
		LiveCoding->Compile();
	}
	else
	{
		NextTickHandle.Reset();
		NextTickHandle = FTSTicker::GetCoreTicker().AddTicker(
			FTickerDelegate::CreateLambda(
				[](float) -> bool
			{
				NextTickHandle.Reset();
				OnComplete();
				return true;
			}
		));
	}

	CacheTableAsset = NewObject<UTempTableAsset>(InParent, InClass, InName, Flags);
	return CacheTableAsset;
}

void UXLSXFactory::OnComplete()
{
	CompileHandle.Reset();

	if (!IsValid(CacheTableAsset))
	{
		return;
	}

	for (auto& Sheet : CacheSheet)
	{
		if (Sheet.AssetType != EAssetType::Struct)
		{
			continue;
		}

		FString DataTableName = FString::Printf(TEXT("DT_%s"), *Sheet.Name);
		UScriptStruct* RowStruct = FindObjectSafe<UScriptStruct>(FTopLevelAssetPath(Dependency_Module_Name, FName(FString::Printf(TEXT("%sTableRow"), *Sheet.Name))));
		if (nullptr == RowStruct)
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to find row struct. [Name:%s]"), *Sheet.Name);
			continue;
		}

		UPackage* Pkg = CreatePackage(*(PackageName / DataTableName));
		UDataTable* DT = Cast<UDataTable>(StaticFindObject(UObject::StaticClass(), Pkg, *DataTableName));
		if (!DT)
		{
			IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
			UDataTableFactory* Factory = NewObject<UDataTableFactory>();
			Factory->Struct = RowStruct;
			DT = Cast<UDataTable>(AssetTools.CreateAsset(DataTableName, PackageName, UDataTable::StaticClass(), Factory));
		}
		else
		{
			DT->EmptyTable();
			DT->RowStruct = RowStruct;
		}

		if (Sheet.IsEmpty())
		{
			continue;
		}

		for (int32 Index = 0; Index < Sheet.Datas.Num(); ++Index)
		{
			auto& Datas = Sheet.Datas[Index];
			uint8* RowPtr = FDataTableEditorUtils::AddRow(DT, FName(*Datas[0].Get<1>()));
			int32 RowIndex = 0;
			for (TFieldIterator<FProperty> It(DT->RowStruct); It && RowIndex < Datas.Num(); ++It, ++RowIndex)
			{
				auto& [HeaderIndex, Value] = Datas[RowIndex];
				FProperty* BaseProp = *It;
				DataTableUtils::AssignStringToProperty(Value, BaseProp, RowPtr);
			}
		}
	}

	if (ObjectTools::DeleteSingleObject(CacheTableAsset))
	{
		CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
	}
	CacheTableAsset = nullptr;
	CompileHandle.Reset();
}

bool UXLSXFactory::GenerateXLSXSheet(const FString& FileName)
{
	CacheSheet.Empty();
#if PLATFORM_WINDOWS
	try
	{
		OpenXLSX::XLDocument Document(Helper::WStringToString(*FileName));
		if (!Document.isOpen())
		{
			UE_LOG(LogTemp, Error, TEXT("Failed to open the sheet. [%s]"), *FileName);
			return false;
		}

		OpenXLSX::XLWorkbook WorkBook = Document.workbook();

		for (auto const& SheetName : WorkBook.sheetNames())
		{
			XLSXSheet Sheet{ .Name = Helper::UTF8ToTCHARString(SheetName) };
			FString TypeDelimiters = Sheet.Name.Left(1);

			if (TypeDelimiters.Equals(TEXT("!")))
			{
				Sheet.AssetType = EAssetType::Struct;
			}
			else if (TypeDelimiters.Equals(TEXT("@")))
			{
				Sheet.AssetType = EAssetType::Enum;
			}
			else
			{
				continue; // Unknown type
			}
			Sheet.Name = Sheet.Name.RightChop(1);

			OpenXLSX::XLWorksheet WorkSheet = WorkBook.worksheet(SheetName);
			int32 RowCount = WorkSheet.rowCount();

			if (RowCount < 2)
			{
				UE_LOG(LogTemp, Error, TEXT("sheet's row size is %d [%s]"), RowCount, *Sheet.Name);
				continue;
			}

			uint32_t RowStart = 0;
			{	// 헤더 정보 저장
				OpenXLSX::XLRow HeaderRow = WorkSheet.row(++RowStart);
				OpenXLSX::XLRow TypeRow = WorkSheet.row(++RowStart);
				auto HeaderRowIter = HeaderRow.cells().begin();
				auto TypeRowIter = TypeRow.cells().begin();

				Sheet.Headers.Reserve(static_cast<int32>(HeaderRow.cellCount()));

				for (int32 Index = 0; HeaderRowIter != HeaderRow.cells().end(); ++Index, ++HeaderRowIter, ++TypeRowIter)
				{
					auto& Header = HeaderRowIter->value();
					auto& Type = TypeRowIter->value();

					FString HeaderName = Helper::UTF8ToTCHARString(Header.get<std::string>());
					FString TypeName = Helper::UTF8ToTCHARString(Type.get<std::string>());

					XLSXHeader XLSXHeader{ .Name = HeaderName, .Index = Index };

					if (TypeName.Contains(TEXT("bool")))
					{
						XLSXHeader.Type = TEXT("bool");
						XLSXHeader.CellType = ECellType::Bool;
					}
					else if (TypeName.Contains(TEXT("int32")))
					{
						XLSXHeader.Type = TEXT("int32");
						XLSXHeader.CellType = ECellType::Int32;
					}
					else if (TypeName.Contains(TEXT("int64")))
					{
						XLSXHeader.Type = TEXT("int64");
						XLSXHeader.CellType = ECellType::Int64;
					}
					else if (TypeName.Contains(TEXT("float")))
					{
						XLSXHeader.Type = TEXT("float");
						XLSXHeader.CellType = ECellType::Float;
					}
					else if (TypeName.Contains(TEXT("string")))
					{
						XLSXHeader.Type = TEXT("FString");
						XLSXHeader.CellType = ECellType::String;
					}
					else if (TypeName.Contains(TEXT("array")))
					{
						XLSXHeader.CellType = ECellType::TArray;
						FString Param = Helper::ExtractSubstring(TypeName, TEXT("<"), TEXT(">"));
						if (TypeName.Contains(TEXT("bool")))
						{
							XLSXHeader.Type = FString::Printf(TEXT("TArray<bool>"));
							XLSXHeader.SubType = ECellType::Bool;
						}
						else if (TypeName.Contains(TEXT("int32")))
						{
							XLSXHeader.Type = FString::Printf(TEXT("TArray<int32>"));
							XLSXHeader.SubType = ECellType::Int32;
						}
						else if (TypeName.Contains(TEXT("int64")))
						{
							XLSXHeader.Type = FString::Printf(TEXT("TArray<int64>"));
							XLSXHeader.SubType = ECellType::Int64;
						}
						else if (TypeName.Contains(TEXT("float")))
						{
							XLSXHeader.Type = FString::Printf(TEXT("TArray<float>"));
							XLSXHeader.SubType = ECellType::Float;
						}
						else if (TypeName.Contains(TEXT("string")))
						{
							XLSXHeader.Type = FString::Printf(TEXT("TArray<FString>"));
							XLSXHeader.SubType = ECellType::String;
						}
						else
						{
							UE_LOG(LogTemp, Error, TEXT("array in array [TypeName:%s]"), *TypeName);
							continue;
						}
					}
					else if (TypeName.Contains(TEXT("enum")))
					{
						FString Param = Helper::ExtractSubstring(TypeName, TEXT("<"), TEXT(">"));
						XLSXHeader.Type = FString::Printf(TEXT("%s"), *Param);
						XLSXHeader.CellType = ECellType::Enum;
					}
					else if (TypeName.Contains(TEXT("asset")))
					{
						FString Param = Helper::ExtractSubstring(TypeName, TEXT("<"), TEXT(">"));
						XLSXHeader.Type = FString::Printf(TEXT("%s"), *Param);
						XLSXHeader.CellType = ECellType::Asset;
					}
					else if (TypeName.Contains(TEXT("class")))
					{
						FString Param = Helper::ExtractSubstring(TypeName, TEXT("<"), TEXT(">"));
						XLSXHeader.Type = FString::Printf(TEXT("%s"), *Param);
						XLSXHeader.CellType = ECellType::Class;
					}
					else
					{
						continue;
					}

					Sheet.Headers.Emplace(Index, XLSXHeader);
				}
			}	// ~헤더 정보 저장

			if (Sheet.AssetType != EAssetType::Enum)
			{	// 값 정보 저장
				for (int32 i = ++RowStart; i <= RowCount; ++i)
				{
					OpenXLSX::XLRow DataRow = WorkSheet.row(i);
					auto DataRowIter = DataRow.cells().begin();

					TArray<TTuple<int32, FString>> Datas;
					Datas.Reserve(Sheet.Headers.Num());
					for (int32 Index = 0; DataRowIter != DataRow.cells().end(); ++Index, ++DataRowIter)
					{
						// 헤더에 미포함된 데이터 값
						if (!Sheet.Headers.Contains(Index))
						{
							continue;
						}
						const XLSXHeader& Header = Sheet.Headers[Index];
						auto& RowData = DataRowIter->value();
						FString Value = XLSX::GetXLSXValue<FString>(RowData);
					
						if (Header.CellType == ECellType::TArray)
						{
							Value = FString::Printf(TEXT("(%s)"), *Value);
						}
						Datas.Emplace(TTuple<int32, FString>{Index, Value});
					}
					Sheet.Datas.Emplace(Datas);
				}
			}	// ~값 정보 저장

			CacheSheet.Emplace(Sheet);
		}
	}
	catch (std::exception Exception)
	{
		UE_LOG(LogTemp, Error, TEXT("[GenerateXLSXSheet] %s [FileName:%s]"), *Helper::UTF8ToTCHARString(Exception.what()), *FileName);
	}

	if (CacheSheet.IsEmpty())
	{
		return false;
	}

	CacheSheet.Sort([](XLSXSheet const& A, XLSXSheet const& B)
	{
		if (A.AssetType == EAssetType::Enum && B.AssetType == EAssetType::Struct)
		{
			return true;
		}
		else if (A.AssetType == EAssetType::Struct && B.AssetType == EAssetType::Enum)
		{
			return false;
		}
		else
		{
			return false;
		}
	});
	return true;
#else
	return false;
#endif
}

FString UXLSXFactory::GenerateTableDesc(FString const& Filename)
{
	FString TableDesc;
	TableDesc += FString::Printf(TEXT("// File generate"));
	TableDesc += TEXT("\n");
	TableDesc += TEXT("#pragma once");
	TableDesc += TEXT("\n\n");
	TableDesc += TEXT("#include \"CoreMinimal.h\"");
	TableDesc += TEXT("\n");
	TableDesc += TEXT("#include \"Engine/DataTable.h\"");
	TableDesc += TEXT("\n");
	TableDesc += FString::Printf(TEXT("#include \"%s.generated.h\""), *Filename);
	TableDesc += TEXT("\n\n");

	for (auto& Sheet : CacheSheet)
	{
		switch (Sheet.AssetType)
		{
		case EAssetType::Struct:
		{
			TableDesc += FString::Printf(TEXT("USTRUCT(BlueprintType)\n"));
			TableDesc += FString::Printf(TEXT("struct %s F%sTableRow : public FTableRowBase\n"), Dependency_Module_API, *Sheet.Name);
			TableDesc += TEXT("{\n");
			TableDesc += TEXT("	GENERATED_BODY()\n");
			TableDesc += TEXT("\n");

			for (auto& [Index, Header] : Sheet.Headers)
			{
				TableDesc += FString::Printf(TEXT("	UPROPERTY(EditAnywhere, BlueprintReadWrite)\n"));
				if (Header.CellType == ECellType::Asset || Header.CellType == ECellType::Class)
				{
					TableDesc += FString::Printf(TEXT("%s %s;"), *Header.Type, *Header.Name);
				}
				else
				{
					TableDesc += FString::Printf(TEXT("%s* %s;"), *Header.Type, *Header.Name);
				}
				TableDesc += TEXT("\n\n");
			}

			TableDesc += TEXT("\n");
			TableDesc += TEXT("};\n");
		}
		break;

		case EAssetType::Enum:
		{
			TableDesc += FString::Printf(TEXT("UENUM(BlueprintType)\n"));
			TableDesc += FString::Printf(TEXT("enum class %s : uint8\n"), *Sheet.Name);

			for (auto& [Index, Header] : Sheet.Headers)
			{
				TableDesc += FString::Printf(TEXT("%s,\n"), *Header.Name);
			}
			TableDesc += TEXT("};\n");
		}
		break;

		}
	}

	return TableDesc;
}

bool UXLSXFactory::IsCompileOnImportEnabled() const
{
#if WITH_HOT_RELOAD && WITH_LIVE_CODING
	ILiveCodingModule* LiveCoding = FModuleManager::GetModulePtr<ILiveCodingModule>(LIVE_CODING_MODULE_NAME);
	if (!LiveCoding)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find LiveCoding module."));
		return true;
	}

	return LiveCoding->IsCompiling();
#else
	return true;
#endif
}

FString UXLSXFactory::GetTablePath() const
{
	FString TablePath;
	if (!GConfig->GetString(TEXT("/XLSX"), TEXT("TablePath"), TablePath, GEditorIni))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find table path. [Path:%s]"), *TablePath);
		return FString();
	}
	return TablePath;
}
