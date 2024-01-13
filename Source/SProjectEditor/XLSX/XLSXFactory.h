
#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "EditorReimportHandler.h"

#include "XLSXFactory.generated.h"

namespace XLSX
{
	struct XLSXSheet;
}

UCLASS()
class UXLSXFactory : public UFactory, public FReimportHandler
{
	GENERATED_BODY()

public:
	UXLSXFactory();

	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;

	// FReimportHandler interface
	virtual bool CanReimport(UObject* Obj, TArray<FString>& OutFilenames) override;
	virtual void SetReimportPaths(UObject* Obj, const TArray<FString>& NewReimportPaths) override;
	virtual EReimportResult::Type Reimport(UObject* Obj) override;
	// ~FReimportHandler interface

private:
	FDelegateHandle	ComplieHandle;
};
