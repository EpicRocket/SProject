
#include "MyGameInstance.h"
// include Engine
//#include "Engine/Engine.h"
// include Project
#include "SProject.h"
#include "Table/TableSubsystem.h"
#include "TableRepository/StageTableRepository.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyGameInstance)


void UMyGameInstance::Init()
{
	Super::Init();

	bool bTableLoadSuccess = true;
	if (auto Subsystem = UTableSubsystem::Get(); !Subsystem->LoadTable())
	{
		bTableLoadSuccess = false;
		UE_LOG(LogProject, Error, TEXT("테이블 로드에 실패하였습니다."));
	}

	if (bTableLoadSuccess)
	{
		if (auto Repository = UStageTableRepository::Get())
		{
			Repository->Load();
		}
	}
}
