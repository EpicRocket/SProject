// Fill out your copyright notice in the Description page of Project Settings.


#include "SGameplayTags.h"
#include "GameplayTagsManager.h"

FSGameplayTags FSGameplayTags::GameplayTag;

void FSGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
	GameplayTag.AddAllTags(GameplayTagsManager);
	GameplayTagsManager.DoneAddingNativeTags();
}

void FSGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
}

void FSGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}
