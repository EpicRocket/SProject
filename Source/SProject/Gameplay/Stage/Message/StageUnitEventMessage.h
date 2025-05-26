// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "NativeGameplayTags.h"

#include "StageUnitEventMessage.generated.h"

class AStageUnitCharacter;

namespace Stage
{
	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Tag_Gameplay_Stage_Unit_Attack_Player);
}

UCLASS(Abstract, BlueprintType, Blueprintable)
class MY_API UStageUnitEvent : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	AStageUnitCharacter* GetSource() const;

	UFUNCTION(BlueprintPure)
	virtual FGameplayTag GetEventTag() const;

	// TODO: implement virtual functions...

public:
	UPROPERTY(BlueprintReadOnly)
	TWeakObjectPtr<AStageUnitCharacter> Source;

};

USTRUCT(BlueprintType)
struct MY_API FStageUnitEventMessage
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UStageUnitEvent> Event;

};

namespace Stage
{
	MY_API void SendUnitEvent(const UObject* WorldContextObject, UStageUnitEvent* Event);
	
	template<typename T>
	MY_API T* NewUnitEvent(AStageUnitCharacter* Source)
	{
		static EObjectFlags Flags = RF_Public | RF_Transient;
		T* NewEvent = NewObject<T>(Source, NAME_None, Flags);
		if (NewEvent)
		{
			NewEvent->Source = Source;
		}
		return NewEvent;
	}
}

// FIXME: 이거 메세지는 하나로 통일 시켜주던가 해야 할 것 같은데...
USTRUCT(BlueprintType)
struct MY_API FStageUnitAttackPlayerMessage
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<AStageUnitCharacter> SourceUnit;

};
