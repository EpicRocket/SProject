
#pragma once

#include "Factories/Factory.h"
#include "XlsxObjectFactory.generated.h"

UCLASS()
class UXlsxObjectFactory : public UFactory
{
	GENERATED_BODY()

public:
	UXlsxObjectFactory();

	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled);

private:
    //class UUserDefinedEnum* CreateEnum();

    //class UUserDefinedStruct* CreateStruct();

};
