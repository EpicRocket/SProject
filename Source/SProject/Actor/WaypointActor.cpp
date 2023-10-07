
#include "WaypointActor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WaypointActor)

AWaypointActor::AWaypointActor(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

UAbilitySystemComponent* AWaypointActor::GetAbilitySystemComponent() const
{
	return nullptr;
}
