
#include "UI/PrimaryGameLayout.h"

#include "Widgets/CommonActivatableWidgetContainer.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PrimaryGameLayout)


UCommonActivatableWidgetContainerBase* UPrimaryGameLayout::GetLayer(FGameplayTag LayerName)
{
	if (Layers.Contains(LayerName))
	{
		return Layers[LayerName];
	}
	return nullptr;
}

TArray<UCommonActivatableWidgetContainerBase*> UPrimaryGameLayout::GetLayers()
{
	TArray<UCommonActivatableWidgetContainerBase*> Result;
	for (auto& Layer : Layers)
	{
		Result.Emplace(Layer.Value);
	}
	return Result;
}


void UPrimaryGameLayout::RegisterLayer(UPARAM(meta = (Categories = "UI.Layer")) FGameplayTag LayerName, UCommonActivatableWidgetContainerBase* Layer)
{
	if (IsDesignTime())
	{
		return;
	}

	Layer->OnTransitioningChanged.AddUObject(this, &UPrimaryGameLayout::OnWidgetStackTransitioning);
	Layer->SetTransitionDuration(0.0F);
	Layers.Emplace(LayerName, Layer);
}
