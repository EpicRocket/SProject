
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

	UFUNCTION(BlueprintCallable)
	void Exclude(const TArray<TScriptInterface<ISelectableActor>>& Actors);

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
	UFUNCTION(BlueprintCallable)
	static void SelectActor(APlayerController* PC, AActor* Target);

	UFUNCTION(BlueprintCallable)
	static void DeselectActor(APlayerController* PC, AActor* Target);

	UFUNCTION(BlueprintCallable)
	static void ClearSelection(APlayerController* PC);

	UFUNCTION(BlueprintCallable)
	static void ClearSelectionExceptOther(APlayerController* PC, const TSet<AActor*>& Other);

	UFUNCTION(BlueprintPure)
	static bool IsSelected(APlayerController* PC, AActor* Target);
};
