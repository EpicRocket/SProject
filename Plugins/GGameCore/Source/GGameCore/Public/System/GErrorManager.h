
#include "Subsystems/GameInstanceSubsystem.h"
#include "System/GErrorTypes.h"

#include "GErrorManager.generated.h"

UCLASS()
class GGAMECORE_API UGErrorManager : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:

private:
    UPROPERTY()
    TMap<FString, FGErrorInfo> Errs;
};
