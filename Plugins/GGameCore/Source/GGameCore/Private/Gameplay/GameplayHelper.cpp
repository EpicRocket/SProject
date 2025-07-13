// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#include "Gameplay/GameplayHelper.h"
// include Engine
#include "Engine/World.h"
#include "GameFramework/Actor.h"
// include GGameCore
#include "Error/GError.h"
#include "Gameplay/GameplayPlayer.h"
#include "Gameplay/Component/GameplayPlayerComponent.h"
#include "Team/Subsystem/GTeamSubsystem.h"
#include "Team/Interface/IGTeamAgent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayHelper)

/* static */AGameplayPlayer* UGameplayHelper::GetGameplayPlayer(const UObject *WorldContextObject, uint8 TeamID)
{
	if (!WorldContextObject)
	{
		return nullptr;
	}

	auto TeamSubsystem = UWorld::GetSubsystem<UGTeamSubsystem>(WorldContextObject->GetWorld());
	if (!TeamSubsystem)
	{
		return nullptr;
	}

    return TeamSubsystem->GetPlayer(TeamID);
}

/* static */FGErrorInfo UGameplayHelper::GetGameplayPlayerComponentByPC(APlayerController* PC, TSubclassOf<UGameplayPlayerComponent> ComponentClass, UActorComponent*& FindComponent)
{
	FindComponent = nullptr;
	if (!IsValid(PC))
	{
		return GameCore::Throw(GameErr::ACTOR_INVALID, TEXT("APlayerController"));
	}

	auto World = PC->GetWorld();
	if (!World)
	{
		return GameCore::Throw(GameErr::WORLD_INVALID, TEXT("PC"));
	}

	auto Subsystem = World->GetSubsystem<UGTeamSubsystem>();
	if (!Subsystem)
	{
		return GameCore::Throw(GameErr::SUBSYSTEM_INVALID, TEXT("UGTeamSubsystem"));
	}

	IGTeamAgent* TeamAgent = Cast<IGTeamAgent>(PC);
	if (!TeamAgent)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("IGTeamAgent Cast failure"));
	}

	auto Player = Subsystem->GetPlayer(TeamAgent->GetTeamID());
	if (!Player)
	{
		return GameCore::Throw(GameErr::POINTER_INVALID, TEXT("GameplayPlayer isn't found"));
	}

	UClass* Com = *ComponentClass;
	FindComponent = Player->GetComponentByClass(Com);

	return GameCore::Pass();
}

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
