// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Loading/Interface/IGLoadingProcess.h"

/*static*/ bool IGLoadingProcess::ShouldShowLoadingScreen(UObject* TestObject, FString& OutReason)
{
	if (TestObject != nullptr)
	{
		if (auto LoadObserver = Cast<IGLoadingProcess>(TestObject))
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

bool IGLoadingProcess::ShouldShowLoadingScreen(FString& OutReason) const
{
	// implementation
	return false;
}
