
#pragma once

#include "Factories/Factory.h"
#include "XlsxObjectFactory.generated.h"

UCLASS()
class UXlsxObjectFactory : public UFactory
{
    GENERATED_BODY()

public:
    UXlsxObjectFactory();

	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;
};
