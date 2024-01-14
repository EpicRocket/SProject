
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
// include LiveCoding
#include "ILiveCodingModule.h"
// include Project
#include "Helper/SStringHelper.h"
#include "Table/TableAsset.h"
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
	enum class EAssetType : uint8
	{
		None,
		Struct,
		Enum,
	};

	enum class ECellType : uint8
	{
		None,
		Bool,
		Int32,
		Int64,
		Float,
		String,
		TArray,
		Enum,
		Asset,
		Class,
	};

	struct XLSXHeader
	{
		FString Name;
		FString Type;
		ECellType CellType = ECellType::None;
		ECellType SubType;

		int32 Index = INDEX_NONE;
	};

	struct XLSXCell
	{
		int32 HeaderIndex = INDEX_NONE;
		FString Value;
	};

	struct XLSXSheet
	{
		FString Name;
		EAssetType AssetType;

		TSortedMap<int32, XLSXHeader> Headers;
		TMultiMap<int32, XLSXCell> Cells;
	};

	TArray<XLSXSheet> CacheSheet;

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
			FString Value = FString(Proxy.get<std::string>().c_str());
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

	void GenerateXLSXSheet(const FString& FileName, TArray<XLSXSheet>& OutResult)
	{
		OutResult.Empty();
#if PLATFORM_WINDOWS
		try
		{
			OpenXLSX::XLDocument Document(Helper::WStringToString(*FileName));
			if (!Document.isOpen())
			{
				UE_LOG(LogTemp, Error, TEXT("Failed to open the sheet. [%s]"), *FileName);
				return;
			}

			OpenXLSX::XLWorkbook WorkBook = Document.workbook();

			for (auto const& SheetName : WorkBook.sheetNames())
			{
				XLSXSheet Sheet{ .Name = FString(SheetName.c_str()) };
				FString TypeDelimiters = Sheet.Name.Left(1);

				if (TypeDelimiters.Equals(TEXT("!")))
				{
					Sheet.AssetType = EAssetType::Struct;
				}
				else if(TypeDelimiters.Equals(TEXT("@")))
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

						FString HeaderName(Header.get<std::string>().c_str());
						FString TypeName(Type.get<std::string>().c_str());

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
				
				if(Sheet.AssetType != EAssetType::Enum)
				{	// 값 정보 저장
					int32 RowIndex = 0;
					for (int32 i = ++RowStart; i <= RowCount; ++i, ++RowIndex)
					{
						OpenXLSX::XLRow DataRow = WorkSheet.row(i);
						auto DataRowIter = DataRow.cells().begin();

						for (int32 Index = 0; DataRowIter != DataRow.cells().end(); ++Index, ++DataRowIter)
						{
							// 헤더에 미포함된 데이터 값
							if (!Sheet.Headers.Contains(Index))
							{
								continue;
							}
							const XLSXHeader& Header = Sheet.Headers[Index];
							auto& RowData = DataRowIter->value();

							XLSXCell Cell{ .HeaderIndex = Index, .Value = GetXLSXValue<FString>(RowData) };
							Sheet.Cells.Emplace(RowIndex, Cell);
						}
					}
				}	// ~값 정보 저장

				OutResult.Emplace(Sheet);
			}
		}
		catch (std::exception Exception)
		{
			UE_LOG(LogTemp, Error, TEXT("[GenerateXLSXSheet] %s [FileName:%s]"), *Helper::UTF8ToTCHARString(Exception.what()), *FileName);
		}
#endif
	};
} // XLSX

//////////////////////////////////////////////////////////////////////////
// UXLSXFactory
//////////////////////////////////////////////////////////////////////////

UXLSXFactory::UXLSXFactory()
{
	SupportedClass = UTableAsset::StaticClass();
	Formats.Add(TEXT("xlsx;Microsoft Excel Spreadsheet"));
	bEditorImport = true;
	bText = false;
}

UObject* UXLSXFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
#if WITH_HOT_RELOAD && WITH_LIVE_CODING
	ILiveCodingModule* LiveCoding = FModuleManager::GetModulePtr<ILiveCodingModule>(LIVE_CODING_MODULE_NAME);
	if (LiveCoding == nullptr)
	{
		return nullptr;
	}
	if (LiveCoding->IsCompiling())
	{
		return nullptr;
	}

	UTableAsset* TableAsset = Cast<UTableAsset>(StaticFindObject(UTableAsset::StaticClass(), InParent, *InName.ToString()));
	if (!TableAsset)
	{
		TableAsset = NewObject<UTableAsset>(InParent, InClass, InName, Flags | RF_Transactional);
	}

	FString TablePath;
	if (!GConfig->GetString(TEXT("/XLSX"), TEXT("TablePath"), TablePath, GEditorIni))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find table path. [Path:%s]"), *TablePath);
		return nullptr;
	}

	XLSX::GenerateXLSXSheet(Filename, XLSX::CacheSheet);

	if (XLSX::CacheSheet.Num() == 0)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to generate sheet. [Path:%s]"), *Filename);
		return nullptr;
	}

	XLSX::CacheSheet.Sort([](XLSX::XLSXSheet const& A, XLSX::XLSXSheet const& B)
		{
			if (A.AssetType == XLSX::EAssetType::Enum && B.AssetType == XLSX::EAssetType::Struct)
			{
				return true;
			}
			else if (A.AssetType == XLSX::EAssetType::Struct && B.AssetType == XLSX::EAssetType::Enum)
			{
				return false;
			}
			else
			{
				return false;
			}
		});

	FString TableDesc;
	TableDesc += FString::Printf(TEXT("// File generate %s"), *FDateTime::Now().ToString());
	TableDesc += TEXT("\n");
	TableDesc += TEXT("#pragma once");
	TableDesc += TEXT("\n\n");
	TableDesc += TEXT("#include \"CoreMinimal.h\"");
	TableDesc += TEXT("\n");
	TableDesc += TEXT("#include \"Engine/DataTable.h\"");
	TableDesc += TEXT("\n");
	TableDesc += FString::Printf(TEXT("#include \"%s.generated.h\""), *InName.ToString());
	TableDesc += TEXT("\n\n");

	for (auto& Sheet : XLSX::CacheSheet)
	{
		switch (Sheet.AssetType)
		{
		case XLSX::EAssetType::Struct:
		{
			TableDesc += FString::Printf(TEXT("USTRUCT(BlueprintType)\n"));
			TableDesc += FString::Printf(TEXT("struct %s F%sTableRow : public FTableRowBase\n"), Dependency_Module_API, *Sheet.Name);
			TableDesc += TEXT("{\n");
			TableDesc += TEXT("	GENERATED_BODY()\n");
			TableDesc += TEXT("\n");
			
			for (auto& [Index, Header] : Sheet.Headers)
			{
				TableDesc += FString::Printf(TEXT("	UPROPERTY(EditAnywhere, BlueprintReadWrite)\n"));
				TableDesc += FString::Printf(TEXT("	%s %s;"), *Header.Type, *Header.Name);
				TableDesc += TEXT("\n\n");
			}

			TableDesc += TEXT("\n");
			TableDesc += TEXT("};\n");

			FString DataTableName = FString::Printf(TEXT("DT_%s"), *Sheet.Name);
			FString DataTablePath = TEXT("/Game") / TablePath;

			if (UScriptStruct* RowStruct = FindObjectSafe<UScriptStruct>(FTopLevelAssetPath(Dependency_Module_Name, FName(FString::Printf(TEXT("%sTableRow"), *Sheet.Name)))))
			{
				IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();

				UDataTableFactory* Factory = NewObject<UDataTableFactory>();
				Factory->Struct = RowStruct;
				UDataTable* NewDT = Cast<UDataTable>(AssetTools.CreateAsset(DataTableName, DataTablePath, UDataTable::StaticClass(), Factory));
				TableAsset->Tables.Emplace(DataTableName, NewDT);
			}

		}
		break;

		case XLSX::EAssetType::Enum:
		{
			TableDesc += FString::Printf(TEXT("UENUM(BlueprintType)\n"));
			TableDesc += FString::Printf(TEXT("enum class %s : uint8\n"), *Sheet.Name);
			TableDesc += TEXT("{\n");
			TableDesc += TEXT("	GENERATED_BODY()\n");
			TableDesc += TEXT("\n");

			for (auto& [Index, Header] : Sheet.Headers)
			{
				TableDesc += FString::Printf(TEXT("	%s,\n"), *Header.Name);
			}
			TableDesc += TEXT("};\n");
		}
		break;

		}
	}

	FString AbsPath = FPaths::ProjectDir() / TEXT("Source") / Dependency_Module / TablePath;
	if (!FPaths::DirectoryExists(AbsPath))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to find module path. [Path:%s]"), *AbsPath);
		return nullptr;
	}
	FString FilePath = AbsPath / FString::Printf(TEXT("%s.h"), *InName.ToString());

	FFileHelper::SaveStringToFile(TableDesc, *FilePath);

	if (ComplieHandle.IsValid())
	{
		LiveCoding->GetOnPatchCompleteDelegate().Remove(ComplieHandle);
	}

	ComplieHandle = LiveCoding->GetOnPatchCompleteDelegate().AddLambda(
		[this, TableAsset, TablePath, Flags, InParent]()
		{
			IAssetTools& AssetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
			ILiveCodingModule* LiveCoding = FModuleManager::GetModulePtr<ILiveCodingModule>(LIVE_CODING_MODULE_NAME);

			LiveCoding->GetOnPatchCompleteDelegate().Remove(ComplieHandle);

			if (!IsValid(TableAsset))
			{
				UE_LOG(LogTemp, Error, TEXT("TableAsset is not valid."));
				return;
			}

			for (auto& Sheet : XLSX::CacheSheet)
			{
				if (Sheet.AssetType != XLSX::EAssetType::Struct)
				{
					continue;
				}

				FString DataTableName = FString::Printf(TEXT("DT_%s"), *Sheet.Name);
				FString DataTablePath = TEXT("/Game") / TablePath;

				if (UScriptStruct* RowStruct = FindObjectSafe<UScriptStruct>(FTopLevelAssetPath(Dependency_Module_Name, FName(FString::Printf(TEXT("%sTableRow"), *Sheet.Name)))))
				{
					UDataTableFactory* Factory = NewObject<UDataTableFactory>();
					Factory->Struct = RowStruct;
					UDataTable* NewDT = Cast<UDataTable>(AssetTools.CreateAsset(DataTableName, DataTablePath / DataTableName, UDataTable::StaticClass(), Factory));
					TableAsset->Tables.Emplace(DataTableName, NewDT);

					TArray<int32> Keys;
					Sheet.Cells.GenerateKeyArray(Keys);
					/*for (auto& Key : Keys) 
					{
						TArray<XLSX::XLSXCell> Cells;
						Sheet.Cells.MultiFind(Key, Cells, true);
						for (auto& Cell : Cells)
						{
							uint8* RowPtr = FDataTableEditorUtils::AddRow(DataTable, FName(*FString::Printf(TEXT("%d"), Key)));
							for (TFieldIterator<FProperty> It(DataTable->RowStruct); It; ++It)
							{
								FProperty* BaseProp = *It;
								check(BaseProp);

								XLSX::XLSXHeader& Header = Sheet.Headers[Cell.HeaderIndex];

								switch (Header.CellType)
								{
								case XLSX::ECellType::Bool:
								case XLSX::ECellType::Int32:
								case XLSX::ECellType::Int64:
								case XLSX::ECellType::Float:
								case XLSX::ECellType::String:
								case XLSX::ECellType::Enum:
								case XLSX::ECellType::TArray:
								{
									DataTableUtils::AssignStringToProperty(Cell.Value, BaseProp, RowPtr);
								}
								break;

								case XLSX::ECellType::Asset:
								{
									FSoftObjectPath	SoftObjectPath(Cell.Value);
									DataTableUtils::AssignStringToProperty(SoftObjectPath.GetLongPackageName(), BaseProp, RowPtr);
								}
								break;

								case XLSX::ECellType::Class:
								{
									FSoftClassPath	SoftClassPath(Cell.Value);
									DataTableUtils::AssignStringToProperty(SoftClassPath.GetLongPackageName(), BaseProp, RowPtr);
								}

								break;
								}
							}
						}
					}*/

					//FAssetRegistryModule::AssetCreated(DataTable);
					//GEditor->BroadcastObjectReimported(DataTable);
				}
			};
		}
	);
	
	//LiveCoding->Compile();

	return TableAsset;
#else
	return nullptr;
#endif
}

bool UXLSXFactory::CanReimport(UObject* Obj, TArray<FString>& OutFilenames)
{
	if (UTableAsset* TableAsset = Cast<UTableAsset>(Obj))
	{
		return true;
	}
	return false;
}

void UXLSXFactory::SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths)
{
}

EReimportResult::Type UXLSXFactory::Reimport(UObject* Obj)
{
	UTableAsset* TableAsset = Cast<UTableAsset>(Obj);
	if (!TableAsset)
	{
		return EReimportResult::Failed;
	}
	return EReimportResult::Succeeded;
}
