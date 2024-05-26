
#include "UI/PrimaryGameLayout.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PrimaryGameLayout)


UCommonActivatableWidgetContainerBase* UPrimaryGameLayout::GetLayerWidget(FGameplayTag LayerName)
{
	if (Layers.Contains(LayerName))
	{
		return Layers[LayerName];
	}
	return nullptr;
}
