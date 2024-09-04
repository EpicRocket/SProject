
#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "EditorReimportHandler.h"

#include "XLSXFactory.generated.h"

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
	};

	enum class ECellType : uint8
	{
		None,
		Bool,
		Int32,
		Int64,
		Float,
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

	struct FXLSXSheet
	{
		FString Name;
		EAssetType AssetType;

		TArray<FXLSXHeader> Headers;
		TArray<TArray<FString>> Datas;

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

	FString GenerateTableDesc(FString const& Filename);

	bool IsCompileOnImportEnabled() const;

	FString GetTablePath() const;
};
