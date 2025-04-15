
#include "GameplayHelper.h"
// include Engine
#include "GameFramework/Actor.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayHelper)

/* static */FGErrorInfo UGameplayHelper::SetGameplayTagByInt32(AActor* Actor, FName TagName, int32 TagValue)
{
	if (Actor == nullptr)
	{
		return GameCore::Throw(GameErr::ACTOR_INVALID);
	}

	FName* FoundTag = Actor->Tags.FindByPredicate([TagName, TagValue](const FName& Tag)
		{
			FString TagString = Tag.ToString();
			FString Left, Right;
			if (!TagString.Split(TEXT("."), &Left, &Right))
			{
				return false;
			}
			if (!Left.Contains(TagName.ToString()))
			{
				return false;
			}
			return true;
		});

	FName NewTag = *FString::Printf(TEXT("%s.%d"), *TagName.ToString(), TagValue);
	if (FoundTag)
	{
		*FoundTag = NewTag;
	}
	else
	{
		Actor->Tags.Add(NewTag);
	}

	return GameCore::Pass();
}

/* static */FGErrorInfo UGameplayHelper::GetGameplayTagByInt32(AActor* Actor, FName TagName, int32& TagValue)
{
	if (Actor == nullptr)
	{
		return GameCore::Throw(GameErr::ACTOR_INVALID);
	}

	for (const auto& Tag : Actor->Tags)
	{
		FString TagString = Tag.ToString();
		FString Left, Right;
		if (!TagString.Split(TEXT("."), &Left, &Right))
		{
			continue;
		}
		if (!Left.Contains(TagName.ToString()))
		{
			continue;
		}
		TagValue = FCString::Atoi(*Right);
		return GameCore::Pass();
	}

	return GameCore::Throw(GameErr::VALUE_INVALID);
}

/* static */bool UGameplayHelper::IsFarFrom(const FVector& A, const FVector& B, float SourceRadius)
{
	return FVector::Dist(A, B) > SourceRadius;
}
