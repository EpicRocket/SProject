// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "Engine/DataAsset.h"

#include "GameplayDataAsset.generated.h"

class UGameFeatureAction;

UCLASS(Blueprintable, Const, Category = "Gameplay", ClassGroup = "Gameplay")
class GGAMECORE_API UGameplayDataAsset : public UPrimaryDataAsset
{
    GENERATED_BODY()

public:
    // UObject
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	// ~UObject

    // UPrimaryDataAsset
#if WITH_EDITORONLY_DATA
	virtual void UpdateAssetBundleData() override;
#endif
    // ~UPrimaryDataAsset

public:
	UPROPERTY(EditDefaultsOnly, Instanced, Category = "Actions")
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

};
