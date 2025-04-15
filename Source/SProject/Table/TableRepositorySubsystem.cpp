
#include "TableRepositorySubsystem.h"
// include Engine
#include "Subsystems/SubsystemCollection.h"
// include Project
#include "TableSubsystem.h"

void UTableRepositorySubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	if (auto Subsystem = Collection.InitializeDependency<UTableSubsystem>())
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

void UTableRepositorySubsystem::Load()
{
	bLoaded = true;
}

void UTableRepositorySubsystem::Unload()
{
	bLoaded = false;
}

void UTableRepositorySubsystem::Patch()
{
	if (bLoaded)
	{
		Load();
	}
}
