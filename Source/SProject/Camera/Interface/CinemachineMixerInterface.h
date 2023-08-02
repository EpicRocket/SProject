

#pragma once

#include "CinemachineCameraInterface.h"
#include "CinemachineMixerInterface.generated.h"

/** 이 Interface를 상속시킬 때는 반드시 ICinemachineCameraInterface도 같이 상속 시켜주세요 */
UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UCinemachineMixerInterface : public UInterface
{
	GENERATED_BODY()
};

class ICinemachineMixerInterface
{
	GENERATED_BODY()

public:
	/** 이 CV카메라가 라이브 중인 카메라의 자식인지 여부 */
	virtual bool IsLiveChild(ICinemachineCameraInterface* ICamera, bool DominantChildOnly = false) const = 0;
};

FORCEINLINE bool IsCinemachineCameraValid(ICinemachineMixerInterface* ICamera)
{
	if (IsValid(Cast<UObject>(ICamera)))
	{
		return false;
	}
	ICinemachineCameraInterface* ICameraInterface = Cast<ICinemachineCameraInterface>(ICamera);
	return ICameraInterface != nullptr && ICameraInterface->IsCameraValid();
}

FORCEINLINE TScriptInterface<ICinemachineMixerInterface> GetCinemachineMixerInterface(ICinemachineMixerInterface* ICamera)
{
	TScriptInterface<ICinemachineMixerInterface> Result = nullptr;
	if (IsCinemachineCameraValid(ICamera))
	{
		Result.SetObject(Cast<UObject>(ICamera));
		Result.SetInterface(ICamera);
	}
	return Result;
}

template<typename T, typename = typename std::enable_if<std::is_base_of<ICinemachineMixerInterface, T>::value && std::is_base_of<UObject, T>::value>::type>
FORCEINLINE TScriptInterface<ICinemachineMixerInterface> GetCinemachineMixerInterface(T* Object)
{
	TScriptInterface<ICinemachineMixerInterface> Result = nullptr;
	if (IsValid(Object))
	{
		Result.SetObject(Object);
		Result.SetInterface(Cast<ICinemachineMixerInterface>(Object));
	}
	return Result;
}
