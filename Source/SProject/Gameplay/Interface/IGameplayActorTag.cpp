
#include "IGameplayActorTag.h"
// include Engine
#include "GameFramework/Actor.h"
// include Project
#include "Gameplay/GameplayHelper.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IGameplayActorTag)

const FName IGameplayActorTag::PositionTagName = TEXT("Position");

void IGameplayActorTag::SetPosition(int32 InPosition)
{
	UGameplayHelper::SetGameplayTagByInt32(Cast<AActor>(this), PositionTagName, InPosition);
}

int32 IGameplayActorTag::GetPosition() const
{
	const AActor* ThisActor = Cast<AActor>(this);
	int32 Position = INDEX_NONE;
	UGameplayHelper::GetGameplayTagByInt32(const_cast<AActor*>(ThisActor), PositionTagName, Position);
	return Position;
}
