
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Templates/SharedPointer.h"

#include "DomainSubsystem.generated.h"

enum class EDomainType : uint8;
struct FDomainBuilding;

/*
 * 유저의 영지 시스템을 관리하는 서브시스템 클래스
 */
UCLASS()
class SPROJECT_API UDomainSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()

public:


private:
    TMap<EDomainType, TSharedPtr<FDomainBuilding>> DomainBuildings;
};
