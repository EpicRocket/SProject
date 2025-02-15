
#include "IGameplayActorTag.h"
#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(IGameplayActorTag)

const FString IGameplayActorTag::DotString = TEXT(".");
const FString IGameplayActorTag::PositionTagName = TEXT("Position");

int32 IGameplayActorTag::GetPosition() const
{
	const AActor* ThisActor = Cast<AActor>(this);
	if (!ThisActor)
	{
		return INDEX_NONE;
	}

	int32 Position = INDEX_NONE;
	for (const auto& Tag : ThisActor->Tags)
	{
		FString TagString = Tag.ToString();
		FString Left, Right;
		if (!TagString.Split(DotString, &Left, &Right))
		{
			continue;
		}

		if (!Left.Contains(PositionTagName))
		{
			continue;
		}

		Position = FCString::Atoi(*Right);
		break;
	}

	return Position;
}
