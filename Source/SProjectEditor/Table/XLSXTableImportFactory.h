
#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Factories/Factory.h"
#include "Factories/ImportSettings.h"
#include "EditorReimportHandler.h"

#include "XLSXTableImportFactory.generated.h"

class UDataTable;

DECLARE_LOG_CATEGORY_EXTERN(LogXLSXTableImporter, Log, All);

USTRUCT()
struct FXLSXImportSheetData
{
	GENERATED_BODY()

	FString SheetName;
	TArray<TArray<FString>> Rows;
};

USTRUCT(BlueprintType)
struct FXLSXTableImportSettings
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Misc")
	TObjectPtr<UScriptStruct> ImportRowStruct;

	FString DataToImport;
};

UCLASS(hidecategories = Object, MinimalAPI)
class UXLSXTableImportFactory : public UFactory, public IImportSettingsParser
{
	GENERATED_BODY()

public:
    UXLSXTableImportFactory();

    virtual FText GetDisplayName() const override;
	virtual bool DoesSupportClass(UClass* Class) override;
	virtual bool FactoryCanImport(const FString& Filename) override;
	virtual IImportSettingsParser* GetImportSettingsParser() override;
	virtual void CleanUp() override;
	virtual void ParseFromJson(TSharedRef<class FJsonObject> ImportSettingsJson) override;
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
	virtual UObject* FactoryCreateText(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, const TCHAR* Type, const TCHAR*& Buffer, const TCHAR* BufferEnd, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

    EReimportResult::Type ReimportCSV(UObject* Obj);

private:
    EReimportResult::Type Reimport(UObject* Obj, const FString& Path);
	TArray<FString> DoImportDataTable(const FXLSXTableImportSettings& ImportSettings, class UDataTable* TargetDataTable);
	FString ConvertXLSXToJsonString(const FString& Filename);

public:
	UPROPERTY(BlueprintReadWrite, Category = "Automation")
	FXLSXTableImportSettings AutomatedImportSettings;

	/** 媛?몄삤湲??듭뀡???쒖떆?섎뒗 ???ъ슜???꾩떆 ?곗씠???뚯씠釉?*/
	UPROPERTY()
	TObjectPtr<UDataTable> DataTableImportOptions;

private:
	bool bImportAll = false;
    
};
