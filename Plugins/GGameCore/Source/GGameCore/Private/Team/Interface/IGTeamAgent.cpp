
#include "Team/Interface/IGTeamAgent.h"
// include Engine
#include "Engine/World.h"
// include GameCore
#include "Team/GTeamTypes.h"
#include "Team/Subsystem/GTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IGTeamAgent)

ETeamAttitude::Type IGTeamAgent::GetTeamAttitudeTowards(const AActor& Other) const
{


	return ETeamAttitude::Type();
}

FGTeamTracker IGTeamAgent::GetTeamTracker() const
{
	const UObject* ThisObject = Cast<UObject>(this);
	if (!ThisObject)
	{
		return FGTeamTracker{};
	}

	auto TeamSubsystem = UWorld::GetSubsystem<UGTeamSubsystem>(ThisObject->GetWorld());
	if (!TeamSubsystem)
	{
		return FGTeamTracker{};
	}

	return TeamSubsystem->GetTeamTracker(GetGenericTeamId());
}
