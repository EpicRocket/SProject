
#pragma once

#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystem/Attribute/GAttributeSet.h"
#include "Misc/EnumRange.h"

#include "StageUnitAttributeSet.generated.h"

UENUM(BlueprintType)
enum class EStageUnitAttribute : uint8
{
	None = 0,

	Level = 1,
	Grade = 2,
	Attack = 3,
	Defence = 4,
	MaxHp = 5,
	Hp = 6,
	AttackSpeed = 7,
	MovementSpeed = 8,
	Range = 9,
	SplashScale = 10,

	Max UMETA(Hidden),
};
ENUM_RANGE_BY_COUNT(EStageUnitAttribute, EStageUnitAttribute::Max)

UCLASS()
class MY_API UStageUnitAttributeSet : public UGAttributeSet
{
	GENERATED_BODY()

public:
	UStageUnitAttributeSet();

	ATTRIBUTE_ACCESSORS(UStageUnitAttributeSet, Level);
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Level;

	ATTRIBUTE_ACCESSORS(UStageUnitAttributeSet, Grade);
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Grade;

	ATTRIBUTE_ACCESSORS(UStageUnitAttributeSet, Attack);
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Attack;

	ATTRIBUTE_ACCESSORS(UStageUnitAttributeSet, Defence);
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Defence;

	ATTRIBUTE_ACCESSORS(UStageUnitAttributeSet, MaxHp);
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MaxHp;

	ATTRIBUTE_ACCESSORS(UStageUnitAttributeSet, Hp);
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Hp;

	ATTRIBUTE_ACCESSORS(UStageUnitAttributeSet, AttackSpeed);
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData AttackSpeed;

	ATTRIBUTE_ACCESSORS(UStageUnitAttributeSet, MovementSpeed);
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData MovementSpeed;

	ATTRIBUTE_ACCESSORS(UStageUnitAttributeSet, Range);
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData Range;

	ATTRIBUTE_ACCESSORS(UStageUnitAttributeSet, SplashScale);
	UPROPERTY(BlueprintReadOnly)
	FGameplayAttributeData SplashScale;
};

UCLASS()
class MY_API UStageUnitAttributeHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static FGameplayAttribute ConvertAttribute(EStageUnitAttribute Attribute);

	UFUNCTION(BlueprintCallable)
	static EStageUnitAttribute ConvertAttributeType(FGameplayAttribute Attribute);

};
