
#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "EditorReimportHandler.h"

#include "XLSXFactory.generated.h"

namespace OpenXLSX {
	class XLWorksheet;
}

UCLASS(BlueprintType)
class UTempTableAsset : public UObject
{
	GENERATED_BODY()
};

UCLASS()
class UXLSXFactory : public UFactory
{
	GENERATED_BODY()

public:
	enum class EAssetType : uint8
	{
		None,
		Struct,
		Enum,
		Constant,
	};

	enum class ECellType : uint8
	{
		None,
		Bool,
		Int32,
		Int64,
		Float32,
		Float64,
		String,
		Text,
		TArray,
		Enum,
		Asset,
		Class,
	};

	struct FXLSXHeader
	{
		FString Name;
		FString Type;
		ECellType CellType = ECellType::None;
		ECellType SubType;

		int32 Index = INDEX_NONE;
	};

	struct FXLSXConst
	{
		FString Key;
		FString Type;
		FString Value;
	};

	struct FXLSXSheet
	{
		FString Name;
		EAssetType AssetType;

		TArray<FXLSXHeader> Headers;
		TArray<TArray<FString>> Datas;
		TArray<FXLSXConst> Consts;

		bool IsEmpty() const
		{
			return Datas.IsEmpty();
		}
	};

public:
	UXLSXFactory();

	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

private:
	static void OnComplete();

	bool GenerateXLSXSheet(const FString& FileName);

	bool GenerateConst(OpenXLSX::XLWorksheet WorkSheet, FXLSXSheet& Sheet);

	void GenerateTableDesc(FString const& Filename);

	FString GetTablePath() const;
};
