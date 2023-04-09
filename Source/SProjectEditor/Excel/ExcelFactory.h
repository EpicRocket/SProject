
#pragma once

#include "Factories/Factory.h"
#include "ExcelFactory.generated.h"


UCLASS()
class UExcelFactory : public UFactory
{
    GENERATED_BODY()

	enum Type
	{
		Structure,
		Enum,
	};

public:
    UExcelFactory();

    //~ Begin UFactory Interface
	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
    //~ End UFactory Interface
};
