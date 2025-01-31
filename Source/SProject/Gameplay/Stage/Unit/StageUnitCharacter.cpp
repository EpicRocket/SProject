
#include "StageUnitCharacter.h"

// include Project
#include "Attribute/StageUnitAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageUnitCharacter)

AStageUnitCharacter::AStageUnitCharacter()
{
	CreateDefaultSubobject<UStageUnitAttributeSet>(TEXT("AttributeSet"));
}
