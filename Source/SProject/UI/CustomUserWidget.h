

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "CustomUserWidget.generated.h"

UCLASS(Abstract, meta = (Category = "Project Only"))
class UCustomUserWidget : public UCommonUserWidget
{
    GENERATED_BODY()

public:
    UCustomUserWidget();
};
