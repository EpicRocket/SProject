
#include "ExcelFactory.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Engine/UserDefinedStruct.h"
#include "UserDefinedStructure/UserDefinedStructEditorData.h"
#include "Kismet2/StructureEditorUtils.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "DataTableEditorUtils.h"
#include "Engine/UserDefinedEnum.h"

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
FString UTF8ToTCHARString(const std::string& utf8Str)
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

	TArray<TCHAR> targetArray; targetArray.Reserve(targetLength);
	MultiByteToWideChar(CP_UTF8, 0, utf8Str.c_str(), -1, targetArray.GetData(), targetLength);

	return FString(targetArray.GetData());
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

					FString DataTableName = FString::Printf(TEXT("DT_%s"), *DefaultName);
					UDataTable* NewDataTable = Cast<UDataTable>(CreateOrOverwriteAsset(UDataTable::StaticClass(), InParent, *DataTableName, Flags));
					NewDataTable->RowStruct = NewUserDefinedStruct;

					if (!NewUserDefinedStruct || !NewDataTable)
					{
						// TODO: 에러 코드
						return nullptr;
					}

					NewUserDefinedStruct->EditorData = NewObject<UUserDefinedStructEditorData>(NewUserDefinedStruct, NAME_None, RF_Transactional);
					if (nullptr == NewUserDefinedStruct->EditorData)
					{
						// TODO: 에러 코드
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

					for (int32 Index = 3; Index < RowCount; ++Index)
					{
						OpenXLSX::XLRow DataRow = WorkSheet.row(Index);

						TArray<FString> Collected; Collected.Reserve(TableStructures.Num());
						auto RowDataIter = DataRow.cells().begin();
						for (auto& [Header, Type] : TableStructures)
						{
							auto CellType = RowDataIter->value().type();
							switch (RowDataIter->value().type())
							{
								case OpenXLSX::XLValueType::Empty:
								{
									// TODO: 에러로그
								} continue;

								case OpenXLSX::XLValueType::Boolean:
								{
									Collected.Add(RowDataIter->value().get<bool>() ? TEXT("true") : TEXT("false"));
								} break;

								case OpenXLSX::XLValueType::Integer:
								{
									Collected.Add(FString::Printf(TEXT("%d"), RowDataIter->value().get<int32>()));
								} break;

								case OpenXLSX::XLValueType::Float:
								{
									Collected.Add(FString::Printf(TEXT("%f"), RowDataIter->value().get<float>()));
								} break;

								case OpenXLSX::XLValueType::String:
								{
									auto Message = UTF8ToTCHARString(RowDataIter->value().get<std::string>());
									Collected.Add(FString(Message));
								} break;

								case OpenXLSX::XLValueType::Error:
								{
									// TODO: 에러로그
								} continue;

								default:
								{
									// TODO: 에러로그
								} break;

							}
							++RowDataIter;
						}
						
						if (Collected.Num() == 0)
						{
							continue;
						}

						FName RowName = FName(*Collected[0]);
						uint8* RowPtr = FDataTableEditorUtils::AddRow(NewDataTable, RowName);
						if (nullptr == RowPtr)
						{
							continue;
						}

						int32 CollectedIndex = 0;
						for (TFieldIterator<FProperty> It(NewDataTable->RowStruct); It; ++It)
						{
							FProperty* BaseProp = *It;
							check(BaseProp);
							DataTableUtils::AssignStringToProperty(Collected[CollectedIndex++], BaseProp, RowPtr);
						}
					}
					
					AssetsToSave.Emplace(NewUserDefinedStruct);
					AssetsToSave.Emplace(NewDataTable);
				}
					break;
				case UExcelFactory::Enum:
				{
					FString UserDefinedEnumName = FString::Printf(TEXT("BE_%s"), *DefaultName);
					UUserDefinedEnum* NewUserDefinedEnum = Cast<UUserDefinedEnum>(CreateOrOverwriteAsset(UUserDefinedEnum::StaticClass(), InParent, *UserDefinedEnumName, Flags));

					if (!NewUserDefinedEnum)
					{
						// TODO: 에러 코드
						return nullptr;
					}

					NewUserDefinedEnum->SetMetaData(TEXT("BlueprintType"), TEXT("true"));
					NewUserDefinedEnum->Bind();
					
					TArray<TPair<FName, int64>> EnumValues;
					for (int32 Index = 3; Index < RowCount; ++Index)
					{
						OpenXLSX::XLRow DataRow = WorkSheet.row(Index);

						auto RowDataIter = DataRow.cells().begin();
						for (auto& [Header, Type] : TableStructures)
						{
							auto CellType = RowDataIter->value().type();
							switch (RowDataIter->value().type())
							{
							case OpenXLSX::XLValueType::Empty:
							{
								// TODO: 에러로그
							} continue;

							case OpenXLSX::XLValueType::Boolean:
							{
							} break;

							case OpenXLSX::XLValueType::Integer:
							{
							} break;

							case OpenXLSX::XLValueType::Float:
							{
							} break;

							case OpenXLSX::XLValueType::String:
							{
								auto Message = UTF8ToTCHARString(RowDataIter->value().get<std::string>());
								EnumValues.Emplace(TPair<FName, int64>(FName(*Message), EnumValues.Num()));

							} break;

							case OpenXLSX::XLValueType::Error:
							{
								// TODO: 에러로그
							} continue;

							default:
							{
								// TODO: 에러로그
							} break;

							}
							++RowDataIter;
						}
					}
					NewUserDefinedEnum->SetEnums(EnumValues, UEnum::ECppForm::EnumClass, EEnumFlags::None, /*bAddMaxKeyIfMissing=*/true);
					AssetsToSave.Emplace(NewUserDefinedEnum);
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
