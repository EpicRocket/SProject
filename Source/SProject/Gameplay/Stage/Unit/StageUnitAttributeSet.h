
#pragma once

#include "AbilitySystemComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "AbilitySystem/Attribute/GAttributeSet.h"
#include "Misc/EnumRange.h"

#include "StageUnitAttributeSet.generated.h"

UENUM(BlueprintType)
enum class EStageUnitAttribute : uint8
{
	None			= 0			UMETA(DisplayName = "?놁쓬"),

	Level			= 1			UMETA(DisplayName = "?덈꺼"),
	Grade			= 2			UMETA(DisplayName = "?깃툒"),
	Attack			= 3			UMETA(DisplayName = "怨듦꺽??),
	Defence			= 4			UMETA(DisplayName = "諛⑹뼱??),
	MaxHp			= 5			UMETA(DisplayName = "理쒕?泥대젰"),
	Hp				= 6			UMETA(DisplayName = "?꾩옱泥대젰"),
	AttackSpeed		= 7			UMETA(DisplayName = "怨듦꺽?띾룄"),
	MovementSpeed	= 8			UMETA(DisplayName = "?대룞?띾룄"),
	Range			= 9			UMETA(DisplayName = "?ш굅由?),
	SplashScale		= 10		UMETA(DisplayName = "?ㅽ뵆?섏돩 踰붿쐞"),

	Max							UMETA(Hidden),
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
	UFUNCTION(BlueprintCallable, Category = "?ㅽ뀒?댁?|?댄듃由щ럭??)
	static FGameplayAttribute ConvertAttribute(EStageUnitAttribute Attribute);

	UFUNCTION(BlueprintCallable, Category = "?ㅽ뀒?댁?|?댄듃由щ럭??)
	static EStageUnitAttribute ConvertAttributeType(FGameplayAttribute Attribute);

};
