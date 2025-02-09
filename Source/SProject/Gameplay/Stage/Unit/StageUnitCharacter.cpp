
#include "StageUnitCharacter.h"
// include GameCore
#include "AbilitySystem/GAbilitySystemComponent.h"
// include Project
#include "Gameplay/Stage/StageLogging.h"
#include "Attribute/StageUnitAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(StageUnitCharacter)

AStageUnitCharacter::AStageUnitCharacter()
{
	static FName StageUnitAttributeSetName = TEXT("AttributeSets");
	AttributeSetCache.Emplace(CreateDefaultSubobject<UStageUnitAttributeSet>(StageUnitAttributeSetName));
}

void AStageUnitCharacter::BeginPlay()
{
	Super::BeginPlay();

	InitailizeBaseStats();
}

const UStageUnitAttributeSet* AStageUnitCharacter::GetUnitSet() const
{
	auto UnitSet = AbilitySystemComponent->GetSet<UStageUnitAttributeSet>();
	if (!UnitSet)
	{
		UE_LOGFMT(LogStage, Error, "UnitSet을 찾을 수 없습니다.");
		return nullptr;
	}
	return UnitSet;
}

void AStageUnitCharacter::SetBaseStats(TMap<EStageUnitAttribute, double> Params)
{
	for (auto& [Type, Value] : Params)
	{
		AbilitySystemComponent->SetNumericAttributeBase(UStageUnitAttributeHelper::ConvertAttribute(Type), static_cast<float>(Value));
	}
}
