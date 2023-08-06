// Fill out your copyright notice in the Description page of Project Settings.


#include "SuperGameplayTags.h"
#include "GameplayTagsManager.h"

FSuperGameplayTags FSuperGameplayTags::GameplayTag;

void FSuperGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();
	GameplayTag.AddAllTags(GameplayTagsManager);
	GameplayTagsManager.DoneAddingNativeTags();
}

void FSuperGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
}

void FSuperGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}
