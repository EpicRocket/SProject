// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
 * 
 */
struct FSuperGameplayTags : public FNoncopyable
{
public:
	static const FSuperGameplayTags& Get() { return GameplayTag; }
	static void InitializeNativeTags();

protected:
	void AddAllTags(UGameplayTagsManager& Manager);
	void AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment);
	
private:
	static FSuperGameplayTags GameplayTag;
};
