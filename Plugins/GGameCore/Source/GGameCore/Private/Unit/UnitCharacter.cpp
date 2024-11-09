
#include "Unit/UnitCharacter.h"
#include "AbilitySystem/GAbilitySystemComponent.h"

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

UAbilitySystemComponent* AUnitCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
