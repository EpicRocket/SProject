
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
#if PLATFORM_WINDOWS
// include LiveCoding
#include "ILiveCodingModule.h"
#endif
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

#if PLATFORM_MAC
namespace OpenXLSX {
	class XLWorksheet {
		XLWorksheet() = default;
	};
}
#endif

namespace XLSX
{
#if PLATFORM_WINDOWS
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
#endif
} // XLSX

TArray<UXLSXFactory::FXLSXSheet> CacheSheet;
TSet<FString> ForwardDeclarations;
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
#if PLATFORM_WINDOWS
	if (!GenerateXLSXSheet(Filename))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate sheet. [Path:%s]"), *Filename);
		return nullptr;
	}

	GenerateTableDesc(InName.ToString());

	PackageName = InParent->GetName();
	int32 LastSlashIndex = INDEX_NONE;
	if (PackageName.FindLastChar('/', LastSlashIndex)) {
		PackageName = PackageName.Left(LastSlashIndex + 1);
	}

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

	CacheTableAsset = NewObject<UTempTableAsset>(InParent, InClass, InName, Flags);
	return CacheTableAsset;
#else
	return nullptr;
#endif
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
		if (DT)
		{
			if (ObjectTools::DeleteSingleObject(DT))
			{
				CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
			}
		}

		IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		UDataTableFactory* Factory = NewObject<UDataTableFactory>();
		Factory->Struct = RowStruct;
		DT = Cast<UDataTable>(AssetTools.CreateAsset(DataTableName, PackageName, UDataTable::StaticClass(), Factory));

		if (Sheet.IsEmpty())
		{
			continue;
		}

		for (int32 Index = 0; Index < Sheet.Datas.Num(); ++Index)
		{
			auto& Datas = Sheet.Datas[Index];
			auto& Data = Datas[0];
			uint8* RowPtr = FDataTableEditorUtils::AddRow(DT, FName(*Data));
			int32 RowIndex = 0;
			for (TFieldIterator<FProperty> It(DT->RowStruct); It && RowIndex < Datas.Num(); ++It, ++RowIndex)
			{
				auto& Value = Datas[RowIndex];
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
	ForwardDeclarations.Empty();
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
			FXLSXSheet Sheet{ .Name = Helper::UTF8ToTCHARString(SheetName) };
			FString TypeDelimiters = Sheet.Name.Left(1);
			Sheet.Name = Sheet.Name.RightChop(1);

			if (TypeDelimiters.Equals(TEXT("!")))
			{
				Sheet.AssetType = EAssetType::Struct;
			}
			else if (TypeDelimiters.Equals(TEXT("@")))
			{
				Sheet.AssetType = EAssetType::Enum;
			}
			else if(TypeDelimiters.Equals(TEXT("#")))
			{
				if (!GenerateConst(WorkBook.worksheet(SheetName), Sheet))
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to generate const. [SheetName:%s]"), *Sheet.Name);
					continue;
				}
				Sheet.AssetType = EAssetType::Constant;
				CacheSheet.Emplace(Sheet);
				continue;
			}
			else
			{
				continue; // Unknown type
			}

			OpenXLSX::XLWorksheet WorkSheet = WorkBook.worksheet(SheetName);
			int32 RowCount = WorkSheet.rowCount();

			if (RowCount < 2)
			{
				UE_LOG(LogTemp, Error, TEXT("sheet's row size is %d [%s]"), RowCount, *Sheet.Name);
				continue;
			}

			
			TMap<int32, FString> HeaderNames;
			TMap<FString, FXLSXHeader> Headers;
			TArray<TMap<FString, FString>> HeaderValues;

			// 헤더 정보 저장
			uint32_t RowStart = 0;
			{
				OpenXLSX::XLRow HeaderRow = WorkSheet.row(++RowStart);
				OpenXLSX::XLRow TypeRow = WorkSheet.row(++RowStart);
				auto HeaderRowIter = HeaderRow.cells().begin();
				auto TypeRowIter = TypeRow.cells().begin();

				//Sheet.Headers.Reserve(static_cast<int32>(HeaderRow.cellCount()));
				HeaderNames.Reserve(static_cast<int32>(HeaderRow.cellCount()));
				Headers.Reserve(static_cast<int32>(HeaderRow.cellCount()));

				for (int32 Index = 0; HeaderRowIter != HeaderRow.cells().end(); ++Index, ++HeaderRowIter, ++TypeRowIter)
				{
					auto& Header = HeaderRowIter->value();
					auto& Type = TypeRowIter->value();

					FString HeaderName = Helper::UTF8ToTCHARString(Header.get<std::string>());
					FString TypeName = Helper::UTF8ToTCHARString(Type.get<std::string>());

					if (Headers.Contains(HeaderName))
					{
						// NOTE: 배열일 수도 있다.
						HeaderNames.Emplace(Index, HeaderName);
						continue;
					}

					FXLSXHeader XLSXHeader{ .Name = HeaderName, .Index = Index };

					int32 StartIndex = TypeName.Find(TEXT("<"), ESearchCase::IgnoreCase, ESearchDir::FromStart);
					FString ExactTypeName = StartIndex == INDEX_NONE ? TypeName : TypeName.Left(StartIndex);

					if (ExactTypeName.Contains(TEXT("bool")))
					{
						XLSXHeader.Type = TEXT("bool");
						XLSXHeader.CellType = ECellType::Bool;
					}
					else if (ExactTypeName.Contains(TEXT("int32")))
					{
						XLSXHeader.Type = TEXT("int32");
						XLSXHeader.CellType = ECellType::Int32;
					}
					else if (ExactTypeName.Contains(TEXT("int64")))
					{
						XLSXHeader.Type = TEXT("int64");
						XLSXHeader.CellType = ECellType::Int64;
					}
					else if (ExactTypeName.Contains(TEXT("float32")))
					{
						XLSXHeader.Type = TEXT("float");
						XLSXHeader.CellType = ECellType::Float32;
					}
					else if (ExactTypeName.Contains(TEXT("float64")))
					{
						XLSXHeader.Type = TEXT("double");
						XLSXHeader.CellType = ECellType::Float64;
					}
					else if (ExactTypeName.Contains(TEXT("FString")))
					{
						XLSXHeader.Type = TEXT("FString");
						XLSXHeader.CellType = ECellType::String;
					}
					else if (ExactTypeName.Contains("FText"))
					{
						XLSXHeader.Type = TEXT("FText");
						XLSXHeader.CellType = ECellType::Text;
					}
					else if (ExactTypeName.Contains(TEXT("enum")))
					{
						FString Param = Helper::ExtractSubstring(TypeName, TEXT("<"), TEXT(">"));
						XLSXHeader.Type = FString::Printf(TEXT("%s"), *Param);
						XLSXHeader.CellType = ECellType::Enum;

						ForwardDeclarations.Emplace(FString::Printf(TEXT("enum class %s : uint8"), *XLSXHeader.Type));
					}
					else if (ExactTypeName.Contains(TEXT("asset")))
					{
						FString Param = Helper::ExtractSubstring(TypeName, TEXT("<"), TEXT(">"));
						XLSXHeader.Type = FString::Printf(TEXT("%s"), *Param);
						XLSXHeader.CellType = ECellType::Asset;

						ForwardDeclarations.Emplace(FString::Printf(TEXT("class %s"), *XLSXHeader.Type));
					}
					else if (ExactTypeName.Contains(TEXT("class")))
					{
						FString Param = Helper::ExtractSubstring(TypeName, TEXT("<"), TEXT(">"));
						XLSXHeader.Type = FString::Printf(TEXT("%s"), *Param);
						XLSXHeader.CellType = ECellType::Class;

						ForwardDeclarations.Emplace(FString::Printf(TEXT("class %s"), *XLSXHeader.Type));
					}
					else if (ExactTypeName.Contains(TEXT("array")))
					{
						XLSXHeader.CellType = ECellType::TArray;
						FString Param = Helper::ExtractSubstring(TypeName, TEXT("<"), TEXT(">"));

						if (Param.Contains(TEXT("bool")))
						{
							XLSXHeader.Type = FString::Printf(TEXT("TArray<%s>"), *Param);
							XLSXHeader.SubType = ECellType::Bool;
						}
						else if (Param.Contains(TEXT("int32")))
						{
							XLSXHeader.Type = FString::Printf(TEXT("TArray<%s>"), *Param);
							XLSXHeader.SubType = ECellType::Int32;
						}
						else if (Param.Contains(TEXT("int64")))
						{
							XLSXHeader.Type = FString::Printf(TEXT("TArray<%s>"), *Param);
							XLSXHeader.SubType = ECellType::Int64;
						}
						else if (Param.Contains(TEXT("float32")))
						{
							XLSXHeader.Type = TEXT("TArray<float>");
							XLSXHeader.SubType = ECellType::Float32;
						}
						else if (ExactTypeName.Contains(TEXT("float64")))
						{
							XLSXHeader.Type = TEXT("TArray<double>");
							XLSXHeader.CellType = ECellType::Float64;
						}
						else if (Param.Contains(TEXT("FString")))
						{
							XLSXHeader.Type = FString::Printf(TEXT("TArray<%s>"), *Param);
							XLSXHeader.SubType = ECellType::String;
						}
						else if (Param.Contains("FText"))
						{
							XLSXHeader.Type = FString::Printf(TEXT("TArray<%s>"), *Param);
							XLSXHeader.SubType = ECellType::Text;
						}
						else if (Param.Contains(TEXT("enum")))
						{
							auto SubParam = Helper::ExtractSubstring(Param, TEXT("<"), TEXT(">"));
							XLSXHeader.Type = FString::Printf(TEXT("TArray<%s>"), *SubParam);
							XLSXHeader.CellType = ECellType::Asset;
							ForwardDeclarations.Emplace(FString::Printf(TEXT("enum class %s : uint8"), *SubParam));
						}
						else if (Param.Contains(TEXT("asset")))
						{
							auto SubParam = Helper::ExtractSubstring(Param, TEXT("<"), TEXT(">"));
							XLSXHeader.Type = FString::Printf(TEXT("TArray<%s>"), *SubParam);
							XLSXHeader.CellType = ECellType::Asset;
							ForwardDeclarations.Emplace(FString::Printf(TEXT("class %s"), *SubParam));
						}
						else if (Param.Contains(TEXT("class")))
						{
							auto SubParam = Helper::ExtractSubstring(Param, TEXT("<"), TEXT(">"));
							XLSXHeader.Type = FString::Printf(TEXT("TArray<%s>"), *SubParam);
							XLSXHeader.CellType = ECellType::Class;
							ForwardDeclarations.Emplace(FString::Printf(TEXT("class %s"), *SubParam));
						}
						else
						{
							continue;
						}
					}
					else
					{
						continue;
					}

					HeaderNames.Emplace(Index, HeaderName);
					Headers.Emplace(HeaderName, XLSXHeader);
				}
			}	// ~헤더 정보 저장

			// 데이터 저장
			HeaderValues.Reserve(RowCount);

			for (int32 i = ++RowStart; i <= RowCount; ++i)
			{
				OpenXLSX::XLRow DataRow = WorkSheet.row(i);
				auto DataRowIter = DataRow.cells().begin();
				 
				TMap<FString, FString> Datas;
				for (int32 Index = 0; DataRowIter != DataRow.cells().end(); ++DataRowIter, ++Index)
				{
					// 헤더에 미포함된 데이터 값
					if (!HeaderNames.Contains(Index))
					{
						continue;
					}
					auto const& HeaderName = HeaderNames[Index];
					auto const& Header = Headers[HeaderName];
					auto& HeaderValue = Datas.FindOrAdd(HeaderName, FString{});

					auto& RowData = DataRowIter->value();
					FString Value = XLSX::GetXLSXValue<FString>(RowData);
					if (Header.CellType == ECellType::TArray && !HeaderValue.IsEmpty())
					{
						HeaderValue = FString::Join(TArray<FString>{ HeaderValue, Value }, TEXT(","));
					}
					else
					{
						HeaderValue = Value;
					}
				}
				HeaderValues.Emplace(Datas);
			}
			
			for (auto& [Name, Header] : Headers)
			{
				Sheet.Headers.Emplace(Header);
			}

			for (auto const& Datas : HeaderValues)
			{
				TArray<FString> Values;
				Values.Reserve(Datas.Num());
				for (auto& [Name, Value] : Datas)
				{
					auto const& Header = Headers[Name];
					if (Header.CellType == ECellType::TArray)
					{
						Values.Emplace(FString::Printf(TEXT("(%s)"), *Value));
					}
					else
					{
						Values.Emplace(Value);
					}
				}
				Sheet.Datas.Emplace(Values);
			}

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

	CacheSheet.Sort([](FXLSXSheet const& A, FXLSXSheet const& B)
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

bool UXLSXFactory::GenerateConst(OpenXLSX::XLWorksheet WorkSheet, FXLSXSheet& Sheet)
{
#if PLATFORM_WINDOWS
	int32 RowCount = WorkSheet.rowCount();

	if (RowCount < 2)
	{
		UE_LOG(LogTemp, Error, TEXT("sheet's row size is %d"), RowCount);
		return false;
	}

	bool bValidKey = false;
	bool bValidType = false;
	bool bValidValue = false;

	TMap<int32, FString> Keys;

	uint32_t RowStart = 0;
	{
		OpenXLSX::XLRow ConstRow = WorkSheet.row(++RowStart);
		auto ConstRowIter = ConstRow.cells().begin();

		int32 Index = 0;
		for (auto Iter = ConstRow.cells().begin(); Iter != ConstRow.cells().end(); ++Iter, ++Index)
		{
			auto& Cell = Iter->value();
			auto HeaderName = Helper::UTF8ToTCHARString(Cell.get<std::string>());

			if (HeaderName.Equals("Key"))
			{
				Keys.Emplace(Index, HeaderName);
				bValidKey = true;
			}
			else if (HeaderName.Equals("Type"))
			{
				Keys.Emplace(Index, HeaderName);
				bValidType = true;
			}
			else if (HeaderName.Equals("Value"))
			{
				Keys.Emplace(Index, HeaderName);
				bValidValue = true;
			}
			else
			{
				continue;
			}
		}
	}

	bool bSuccess = true;
	if (!bValidKey)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find key header"));
		bSuccess = false;
	}
	if (!bValidType)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find type header"));
		bSuccess = false;
	}
	if (!bValidValue)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find value header"));
		bSuccess = false;
	}

	if (!bSuccess)
	{
		return false;
	}

	for (int32 i = ++RowStart; i <= RowCount; ++i)
	{
		OpenXLSX::XLRow ConstRow = WorkSheet.row(i);
		FXLSXConst ConstSheet;

		int32 Index = 0;
		for (auto Iter = ConstRow.cells().begin(); Iter != ConstRow.cells().end(); ++Iter, ++Index)
		{
			if (!Keys.Contains(Index))
			{
				continue;
			}

			auto& Cell = Iter->value();
			auto CellValue = XLSX::GetXLSXValue<FString>(Cell);

			if (Keys[Index].Equals(TEXT("Key")))
			{
				ConstSheet.Key = CellValue;
			}
			else if (Keys[Index].Equals(TEXT("Type")))
			{
				ConstSheet.Type = CellValue;
			}
			else if (Keys[Index].Equals(TEXT("Value")))
			{
				ConstSheet.Value = CellValue;
			}
		}

		Sheet.Consts.Emplace(ConstSheet);
	}

	return true;
#else
	return false;
#endif
}

void UXLSXFactory::GenerateTableDesc(FString const& Filename)
{
	auto ModuleIni = FPaths::ProjectConfigDir() / FString::Printf(TEXT("Default%s.ini"), Dependency_Module);
	for (auto& Sheet : CacheSheet)
	{
		auto Section = FString::Printf(TEXT("/Script/%s.%sSettings"), Dependency_Module, *Sheet.Name);
		GConfig->EmptySection(*Section, ModuleIni);

		for (auto& Const : Sheet.Consts)
		{
			if (Const.Type.Contains(TEXT("bool")))
			{
				GConfig->SetBool(
					*Section,
					*Const.Key,
					Const.Value.ToBool(),
					ModuleIni
				);
			}
			else if (Const.Type.Contains(TEXT("int32")))
			{
				GConfig->SetInt(
					*Section,
					*Const.Key,
					FCString::Atoi(*Const.Value),
					ModuleIni
				);
			}
			else if (Const.Type.Contains(TEXT("int64")))
			{
				GConfig->SetInt(
					*Section,
					*Const.Key,
					FCString::Atoi(*Const.Value),
					ModuleIni
				);
			}
			else if (Const.Type.Contains(TEXT("float32")))
			{
				GConfig->SetFloat(
					*Section,
					*Const.Key,
					FCString::Atof(*Const.Value),
					ModuleIni
				);
			}
			else if (Const.Type.Contains(TEXT("float64")))
			{
				GConfig->SetDouble(
					*Section,
					*Const.Key,
					FCString::Atod(*Const.Value),
					ModuleIni
				);
			}
			else if (Const.Type.Contains(TEXT("FString")))
			{
				GConfig->SetString(
					*Section,
					*Const.Key,
					*Const.Value,
					ModuleIni
				);
			}
			else if (Const.Type.Contains(TEXT("FText")))
			{
				GConfig->SetText(
					*Section,
					*Const.Key,
					FText::FromString(Const.Value),
					ModuleIni
				);
			}
		}
	}
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
