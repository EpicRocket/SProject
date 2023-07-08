

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "CinemachineActionActorInterface.generated.h"

/*
 * FreeLook, Lockon 카메라를 사용할 수 있는 액터에 대한 인터페이스
 */
UINTERFACE(BlueprintType)
class UCinemachineActionActorInterface : public UInterface
{
    GENERATED_BODY()
};

class ICinemachineActionActorInterface
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Cinemachine|Interface")
	class UCinemachineVirtualCameraBaseComponent* GetDefaultVirtualCamera();
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Cinemachine|Interface")
	class UCinemachineLockonFreeLookComponent* GetLockonVirtualCamera();
};
