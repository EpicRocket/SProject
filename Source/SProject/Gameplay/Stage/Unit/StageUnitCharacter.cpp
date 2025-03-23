
#include "StageUnitCharacter.h"
// include GameCore
#include "AbilitySystem/GAbilitySystemComponent.h"
// include Project
#include "Gameplay/GameplayUnitAnimState.h"
#include "Gameplay/Stage/StageLogging.h"
#include "StageUnitAttributeSet.h"

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

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(UStageUnitAttributeSet::GetHpAttribute()).AddWeakLambda(
		this,
		[this](const FOnAttributeChangeData& Data)
		{
			if (Data.NewValue <= 0)
			{
				Kill();
			}
			UE_LOGFMT(LogStage, Log, "체력이 변경되었습니다. 변경된 값: {Hp}", Data.NewValue);
		}
	);
}

UAnimMontage* AStageUnitCharacter::GetAnimMontage(UPARAM(meta = (Categories = "AnimState")) FGameplayTag AnimState) const
{
	if (!IsValid(AnimStateDataAsset))
	{
		return nullptr;
	}

	auto Iter = AnimStateDataAsset->AnimStates.Find(AnimState);
	if (!Iter)
	{
		return nullptr;
	}

	if (Iter->AnimMontages.IsEmpty())
	{
		return nullptr;
	}

	return Iter->AnimMontages[FMath::RandRange(0, Iter->AnimMontages.Num() - 1)];
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
