
#include "Team/Interface/IGTeamAgent.h"
// include Engine
#include "Engine/World.h"
// include GameCore
#include "Team/GTeamTypes.h"
#include "Team/Subsystem/GTeamSubsystem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IGTeamAgent)

ETeamAttitude::Type IGTeamAgent::GetTeamAttitudeTowards(const AActor& Other) const
{
	const UObject* ThisObject = Cast<UObject>(this);
	if (!ThisObject)
	{
		return ETeamAttitude::Type::Neutral;
	}

	const IGTeamAgent* OtherTeamAgent = Cast<IGTeamAgent>(&Other);
	if (!OtherTeamAgent)
	{
		return ETeamAttitude::Type::Neutral;
	}

	auto TeamSubsystem = UWorld::GetSubsystem<UGTeamSubsystem>(ThisObject->GetWorld());
	if (!TeamSubsystem)
	{
		return ETeamAttitude::Type::Neutral;
	}

	return TeamSubsystem->GetTeamAttitudeTowards(GetGenericTeamId(), OtherTeamAgent->GetGenericTeamId());
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
