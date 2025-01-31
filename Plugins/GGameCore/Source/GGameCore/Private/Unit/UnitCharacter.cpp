
#include "Unit/UnitCharacter.h"
#include "AbilitySystem/GAbilitySystemComponent.h"
#include "AbilitySystem/Attribute/GBaseAttributeSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(UnitCharacter)

AUnitCharacter::AUnitCharacter()
{
	static FName AbilitySystemComponentName = TEXT("AbilitySystemComponent");
	AbilitySystemComponent = CreateDefaultSubobject<UGAbilitySystemComponent>(AbilitySystemComponentName);
}

void AUnitCharacter::BeginPlay()
{
    Super::BeginPlay();

	auto CharacterMesh = GetMesh();
	if (CharacterMesh && DefaultAnimClassLayer != nullptr)
	{
		CharacterMesh->LinkAnimClassLayers(DefaultAnimClassLayer);
	}
}

void AUnitCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (!IsValid(this))
	{
		return;
	}

	check(AbilitySystemComponent);
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
}

void AUnitCharacter::SetGenericTeamId(const FGenericTeamId& InTeamID)
{
	TeamID = InTeamID.GetId();
}

FGenericTeamId AUnitCharacter::GetGenericTeamId() const
{
	return FGenericTeamId(TeamID);
}

UAbilitySystemComponent* AUnitCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
