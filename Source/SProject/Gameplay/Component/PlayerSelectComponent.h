
#pragma once

#include "Framework/Player/GPlayerComponent.h"
#include "UObject/ScriptInterface.h"

#include "PlayerSelectComponent.generated.h"

class ISelectableActor;

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
