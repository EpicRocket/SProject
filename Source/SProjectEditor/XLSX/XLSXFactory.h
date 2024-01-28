
#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"

#include "XLSXFactory.generated.h"

UCLASS()
class UXLSXFactory : public UFactory
{
	GENERATED_BODY()

public:
	UXLSXFactory();

	virtual UObject* FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled) override;
};

