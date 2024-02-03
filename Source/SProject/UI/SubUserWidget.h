

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"

#include "SubUserWidget.generated.h"

UCLASS(Abstract, meta = (Category = "Project Only"))
class USubUserWidget : public UCommonUserWidget
{
    GENERATED_BODY()

public:
    USubUserWidget();
};
