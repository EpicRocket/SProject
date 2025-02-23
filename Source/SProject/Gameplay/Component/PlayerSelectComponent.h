
#pragma once

#include "Core/Component/GPlayerComponent.h"
#include "UObject/ScriptInterface.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "PlayerSelectComponent.generated.h"

class ISelectableActor;
class APlayerController;
class AActor;

UCLASS(meta = (BlueprintSpawnableComponent))
class MY_API UPlayerSelectComponent : public UGPlayerComponent
{
    GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Select(TScriptInterface<ISelectableActor> Actor);

	UFUNCTION(BlueprintCallable)
	void Deselect(TScriptInterface<ISelectableActor> Actor);

	UFUNCTION(BlueprintCallable)
	void ClearSelection();

	UFUNCTION(BlueprintPure)
	bool IsSelected(TScriptInterface<ISelectableActor> Actor) const;

public:
	UPROPERTY()
	TArray<TScriptInterface<ISelectableActor>> SelectedActors;
};

UCLASS()
class MY_API UPlayerSelectFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "게임플레이|PC|선택")
	static void SelectActor(APlayerController* PC, AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "게임플레이|PC|선택")
	static void DeselectActor(APlayerController* PC, AActor* Target);

	UFUNCTION(BlueprintCallable, Category = "게임플레이|PC|선택")
	static void ClearSelection(APlayerController* PC);

	UFUNCTION(BlueprintPure, Category = "게임플레이|PC|선택")
	static bool IsSelected(APlayerController* PC, AActor* Target);
};
