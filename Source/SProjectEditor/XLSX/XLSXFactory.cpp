
#include "XLSXFactory.h"

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

#include "Helper/SStringHelper.h"
#include "Table/TableAsset.h"

#include <exception>
#include <ranges>
#include <variant>
#if PLATFORM_WINDOWS
#include "OpenXLSX/OpenXLSX.hpp"
#include "OpenXLSX/headers/XLCellValue.hpp"
#endif

#define Dependency_Module_Name TEXT("/Script/SProject")

DECLARE_LOG_CATEGORY_EXTERN(LogFactory, Log, All);

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
		ECellType CellType = ECellType::None;
		ECellType SubType;
		std::variant<UEnum*, UClass*> SubType_Ptr;

		int32 Index = INDEX_NONE;
	};

	struct IXLSXCell
	{
		int32 HeaderIndex = INDEX_NONE;
	};

	template <typename T>
	struct TXLSXCell : public IXLSXCell
	{
		T Value;
	};

	struct XLSXSheet
	{
		FString Name;
		EAssetType AssetType;

		TSortedMap<int32, XLSXHeader> Headers;
		TMultiMap<int32, IXLSXCell> Cells;
	};

	void GenerateXLSXSheet(const FString& FileName, TArray<XLSXSheet>& OutResult)
	{
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
							XLSXHeader.CellType = ECellType::Bool;
						}
						else if (TypeName.Contains(TEXT("int32")))
						{
							XLSXHeader.CellType = ECellType::Int32;
						}
						else if (TypeName.Contains(TEXT("int64")))
						{
							XLSXHeader.CellType = ECellType::Int64;
						}
						else if (TypeName.Contains(TEXT("float")))
						{
							XLSXHeader.CellType = ECellType::Float;
						}
						else if (TypeName.Contains(TEXT("string")))
						{
							XLSXHeader.CellType = ECellType::String;
						}
						else if (TypeName.Contains(TEXT("array")))
						{
							XLSXHeader.CellType = ECellType::TArray;
							FString Param = Helper::ExtractSubstring(TypeName, TEXT("<"), TEXT(">"));
							if (TypeName.Contains(TEXT("bool")))
							{
								XLSXHeader.SubType = ECellType::Bool;
							}
							else if (TypeName.Contains(TEXT("int32")))
							{
								XLSXHeader.SubType = ECellType::Int32;
							}
							else if (TypeName.Contains(TEXT("int64")))
							{
								XLSXHeader.SubType = ECellType::Int64;
							}
							else if (TypeName.Contains(TEXT("float")))
							{
								XLSXHeader.SubType = ECellType::Float;
							}
							else if (TypeName.Contains(TEXT("string")))
							{
								XLSXHeader.SubType = ECellType::String;
							}
							else if (TypeName.Contains(TEXT("enum")))
							{
								XLSXHeader.SubType = ECellType::Enum;

								FTopLevelAssetPath AssetPath(Dependency_Module_Name, *Param);
								UEnum* Enum = FindObject<UEnum>(AssetPath);
								if (nullptr == Enum)
								{
									UE_LOG(LogTemp, Error, TEXT("Failed to find enum. [EnumName:%s]"), *Param);
									continue;
								}
								XLSXHeader.SubType_Ptr = Enum;
							}
							else if (TypeName.Contains(TEXT("asset")))
							{
								XLSXHeader.SubType = ECellType::Asset;
								FTopLevelAssetPath AssetPath(Dependency_Module_Name, *Param);

								UObject* Asset = FindObject<UObject>(AssetPath);
								if (nullptr == Asset)
								{
									UE_LOG(LogTemp, Error, TEXT("Failed to find asset. [AssetName:%s]"), *Param);
									continue;
								}
								XLSXHeader.SubType_Ptr = Asset->GetClass();
							}
							else if (TypeName.Contains(TEXT("class")))
							{
								XLSXHeader.SubType = ECellType::Class;
								FTopLevelAssetPath AssetPath(Dependency_Module_Name, *Param);

								UObject* Asset = FindObject<UObject>(AssetPath);
								if (Asset == nullptr)
								{
									UE_LOG(LogTemp, Error, TEXT("Failed to find class. [AssetName:%s]"), *Param);
									continue;
								}
								XLSXHeader.SubType_Ptr = Asset->GetClass();
							}
							else
							{
								UE_LOG(LogTemp, Error, TEXT("array in array [TypeName:%s]"), *TypeName);
								continue;
							}
						}
						else if (TypeName.Contains(TEXT("enum")))
						{
							XLSXHeader.CellType = ECellType::Enum;
							FString Param = Helper::ExtractSubstring(TypeName, TEXT("<"), TEXT(">"));

							FTopLevelAssetPath AssetPath(Dependency_Module_Name, *FString::Printf(TEXT("E%s"), *Param));
							UEnum* Enum = FindObject<UEnum>(AssetPath);
							if (nullptr == Enum)
							{
								UE_LOG(LogTemp, Error, TEXT("Failed to find enum. [EnumName:%s]"), *Param);
								continue;
							}
							XLSXHeader.SubType_Ptr = Enum;
						}
						else if (TypeName.Contains(TEXT("asset")))
						{
							XLSXHeader.CellType = ECellType::Asset;
							FString Param = Helper::ExtractSubstring(TypeName, TEXT("<"), TEXT(">"));

							FTopLevelAssetPath AssetPath(Dependency_Module_Name, *Param);

							UObject* Asset = FindObject<UObject>(AssetPath);
							if (nullptr == Asset)
							{
								UE_LOG(LogTemp, Error, TEXT("Failed to find class. [ClassName:%s]"), *FString::Printf(TEXT("U%s"), *Param));
								continue;
							}
							XLSXHeader.SubType_Ptr = Asset->GetClass();
						}
						else if (TypeName.Contains(TEXT("class")))
						{
							XLSXHeader.CellType = ECellType::Class;
							FString Param = Helper::ExtractSubstring(TypeName, TEXT("<"), TEXT(">"));

							FTopLevelAssetPath AssetPath(Dependency_Module_Name, *Param);

							UObject* Asset = FindObject<UObject>(AssetPath);
							if (Asset == nullptr)
							{
								UE_LOG(LogTemp, Error, TEXT("Failed to find class. [AssetName:%s]"), *FString::Printf(TEXT("U%s"), *Param));
								continue;
							}
							XLSXHeader.SubType_Ptr = Asset->GetClass();
						}
						else
						{
							continue;
						}

						Sheet.Headers.Emplace(Index, XLSXHeader);
					}
				}	// ~헤더 정보 저장
				
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
							switch (Header.CellType)
							{
							case ECellType::Bool:
							{
								TXLSXCell<bool> Cell{ .Value = RowData.get<bool>() };
								Cell.HeaderIndex = Index;
								Sheet.Cells.Emplace(RowIndex, Cell);
							}
							break;

							case ECellType::Int32:
							{
								TXLSXCell<int32> Cell{ .Value = RowData.get<int32>() };
								Cell.HeaderIndex = Index;
								Sheet.Cells.Emplace(RowIndex, Cell);
							}
							break;

							case ECellType::Int64:
							{
								TXLSXCell<int64> Cell{ .Value = RowData.get<int64>() };
								Cell.HeaderIndex = Index;
								Sheet.Cells.Emplace(RowIndex, Cell);
							}
							break;

							case ECellType::Float:
							{
								TXLSXCell<float> Cell{ .Value = RowData.get<float>() };
								Cell.HeaderIndex = Index;
								Sheet.Cells.Emplace(RowIndex, Cell);
							}
							break;

							case ECellType::String:
							{
								TXLSXCell<FString> Cell{ .Value = FString(RowData.get<std::string>().c_str()) };
								Cell.HeaderIndex = Index;
								Sheet.Cells.Emplace(RowIndex, Cell);
							}
							break;

							case ECellType::TArray:
							{
								// TODO:
							}
							break;

							case ECellType::Enum:
							{
								if (std::holds_alternative<UEnum*>(Header.SubType_Ptr))
								{
									auto Enum = std::get<UEnum*>(Header.SubType_Ptr);

									auto EnumValue = Enum->GetValueByNameString(RowData.get<std::string>().c_str());
									TXLSXCell<int64> Cell{ .Value = EnumValue };
									Cell.HeaderIndex = Index;
									Sheet.Cells.Emplace(RowIndex, Cell);
								}
							}
							break;

							case ECellType::Asset:
							{
								if (std::holds_alternative<UClass*>(Header.SubType_Ptr))
								{
									auto Class = std::get<UClass*>(Header.SubType_Ptr);

									FTopLevelAssetPath AssetPath(Dependency_Module_Name, *FString(RowData.get<std::string>().c_str()));
									if (UObject* DataAsset = StaticFindObject(Class, AssetPath, true))
									{
										TXLSXCell<TSoftObjectPtr<UObject>> Cell{ .Value = TSoftObjectPtr<UObject>(DataAsset->GetClass()) };
										Cell.HeaderIndex = Index;
										Sheet.Cells.Emplace(RowIndex, Cell);
									}
								}
							}
							break;

							case ECellType::Class:
							{
								if (std::holds_alternative<UClass*>(Header.SubType_Ptr))
								{
									auto Class = std::get<UClass*>(Header.SubType_Ptr);

									FTopLevelAssetPath AssetPath(Dependency_Module_Name, *FString(RowData.get<std::string>().c_str()));
									if (UObject* DataAsset = StaticFindObject(Class, AssetPath, true))
									{
										TXLSXCell<TSoftClassPtr<UObject>> Cell{ .Value = TSoftClassPtr<UObject>(DataAsset->GetClass()) };
										Cell.HeaderIndex = Index;
										Sheet.Cells.Emplace(RowIndex, Cell);
									}
								}
							}
							break;

							}
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
	bEditAfterNew = true;
	bEditorImport = true;
	bText = false;
}

UObject* UXLSXFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	UTableAsset* TableAsset = Cast<UTableAsset>(StaticFindObject(nullptr, InParent, *InName.ToString()));
	if (!TableAsset)
	{
		TableAsset = NewObject<UTableAsset>(InParent, InClass, InName, Flags);
	}
		
	if (TableAsset->GetClass()->IsChildOf(InClass))
	{
		TableAsset = NewObject<UTableAsset>(InParent, InClass, InName, Flags, nullptr);
	}

	if (!ObjectTools::DeleteSingleObject(TableAsset))
	{
		UE_LOG(LogFactory, Warning, TEXT("Could not delete existing asset %s"), *TableAsset->GetFullName());
		return nullptr;
	}

	if (TableAsset == nullptr)
	{
		UE_LOG(LogFactory, Error, TEXT("Failed to create table asset. [FileName:%s]"), *Filename);
		return nullptr;
	}



	TArray<XLSX::XLSXSheet> Sheets;
	XLSX::GenerateXLSXSheet(Filename, Sheets);


	return TableAsset;
}
