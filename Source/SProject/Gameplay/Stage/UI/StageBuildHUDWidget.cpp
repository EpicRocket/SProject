
#include "StageBuildHUDWidget.h"
// include Project
#include "Gameplay/Stage/ETC/StageBuildZone.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageBuildHUDWidget)

void UStageBuildHUDWidget::Setup(AStageBuildZone* OwnerBuildZone)
{
	Owner = OwnerBuildZone;

	OnSetup();
}
