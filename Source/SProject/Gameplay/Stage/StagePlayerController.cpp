
#include "StagePlayerController.h"
// include Engine
#include "Blueprint/UserWidget.h"
// include Project
#include "StageLogging.h"
#include "StageGameplayTags.h"
#include "Table/TableSubsystem.h"
#include "User/DomainSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StagePlayerController)

AStagePlayerController* UStagePlayerControllerHelper::GetStagePlayerControllerToWidget(UUserWidget* UserWidget)
{
	if (UserWidget == nullptr)
	{
		return nullptr;
	}
	return UserWidget->GetOwningPlayer<AStagePlayerController>();
}
