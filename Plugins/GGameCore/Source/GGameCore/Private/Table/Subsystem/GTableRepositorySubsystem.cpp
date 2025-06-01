
#include "Table/Subsystem/GTableRepositorySubsystem.h"
// include Engine
#include "Subsystems/SubsystemCollection.h"
// include GameCore
#include "Table/Subsystem/GTableSubsystem.h"

void UGTableRepositorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (auto Subsystem = Collection.InitializeDependency<UGTableSubsystem>())
	{
		Subsystem->TableLoadCompleted.AddWeakLambda(this, [this, ThisPtr = TWeakObjectPtr<ThisClass>(this)]()
			{
				if (ThisPtr.IsValid())
				{
					Patch();
				}
			});
	}
}

void UGTableRepositorySubsystem::Load()
{
	bLoaded = true;
}

void UGTableRepositorySubsystem::Unload()
{
	bLoaded = false;
}

void UGTableRepositorySubsystem::Patch()
{
	if (bLoaded)
	{
		Load();
	}
}
