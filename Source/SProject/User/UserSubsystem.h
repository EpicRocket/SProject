
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Templates/SharedPointer.h"

#include "UserSubsystem.generated.h"


struct FUserInfo;

/** 유저 정보 관리 시스템 */
UCLASS()
class SPROJECT_API UUserSubsystem : public ULocalPlayerSubsystem
{
    GENERATED_BODY()
    
public:
   
private:
    TSharedPtr<FUserInfo> UserInfo;
};
