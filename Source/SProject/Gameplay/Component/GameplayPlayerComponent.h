
#pragma once

#include "Core/Component/GGameCoreComponent.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayPlayerComponent.generated.h"

class AGameplayPlayer;
struct FLatentActionInfo;

UCLASS(Abstract, Blueprintable, BlueprintType, HideCategories = (Trigger, PhysicsVolume))
class MY_API UGameplayPlayerComponent : public UGGameCoreComponent
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, BlueprintPure)
    AGameplayPlayer* GetGameplayPlayer() const;
    
	template <class T>
	T* GetGameplayPlayer() const
	{
		static_assert(TPointerIsConvertibleFromTo<T, AGameplayPlayer>::Value, "GetGameplayPlayer??'T' ?쒗뵆由??뚮씪誘명꽣??AGameplayPlayer?먯꽌 ?뚯깮?섏뼱???⑸땲??");
		return Cast<T>(GetOwner());
	}

};

UCLASS()
class MY_API UGameplayPlayerHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "寃뚯엫?뚮젅???뚮젅?댁뼱", meta = (DeterminesOutputType = "ComponentClass", DynamicOutputParam = "FindComponent"))
	static void GetGameplayPlayerComponentByPC(class APlayerController* PC, TSubclassOf<UGameplayPlayerComponent> ComponentClass, UActorComponent*& FindComponent);

};
