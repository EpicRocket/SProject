// Copyright (c) 2025 Team EpicRocket. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "Containers/Queue.h"

#include "GTableRepositorySubsystem.generated.h"

class UGTableSubsystem;
struct FLatentActionInfo;
struct FSoftObjectPath;
struct FStreamableHandle;

struct FRecyclableIdPool
{
public:
	int32 GetNext()
	{
		int32 Recycled;
		if (FreeIds.Dequeue(Recycled))
		{
			return Recycled;
		}
		return NextId++;
	}

	void Release(int32 Id)
	{
		FreeIds.Enqueue(Id);
	}

private:
	int32 NextId = 0;
	TQueue<int32> FreeIds;

};

UCLASS(Abstract)
class GGAMECORE_API UGTableRepositorySubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Deinitialize() override;

protected:
    void RequestTasks(TArray<FSoftObjectPath> RawAssetList, TUniqueFunction<void()> CompleteCallback);

private:
    TMap<int32, TSharedPtr<FStreamableHandle>> Tasks;
	FRecyclableIdPool TaskIdPool;

};
