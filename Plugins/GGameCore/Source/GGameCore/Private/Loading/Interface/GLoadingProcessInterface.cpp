
#include "Loading/Interface/GLoadingProcessInterface.h"

/*static*/ bool IGLoadingProcessInterface::ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason)
{
	if (TestObject != nullptr)
	{
		if (auto LoadObserver = Cast<IGLoadingProcessInterface>(TestObject))
		{
            FString ObserverReason;
			if (LoadObserver->ShouldShowLoadingScreen(/*out*/ ObserverReason))
			{
				if (ensureMsgf(!ObserverReason.IsEmpty(), TEXT("%s가 로딩 화면을 표시하려는 이유를 설정하지 못했습니다."), *GetPathNameSafe(TestObject)))
				{
					OutReason = ObserverReason;
				}
				return true;
			}
		}
	}

	return false;
}

bool IGLoadingProcessInterface::ShouldShowLoadingScreen(FString& OutReason) const
{
	// implementation
	return false;
}
