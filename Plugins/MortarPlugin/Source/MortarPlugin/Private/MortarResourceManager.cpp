//Copyright(c) 2021 FinalRockstar All Rights Reserved


#include "MortarResourceManager.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraComponent.h"
#include "MortarPlugin.h"


void UMortarResourceManager::Initialize(FSubsystemCollectionBase& Collection)
{
    MaxOneTimeDelete = 10;

    //Handled in Blueprint Which Will Set THe Per ResourceCount map
    InitializeResourceManagement();
    
    //Initialize Map to store  actors
    for (auto& Element : PerResourceCount)
    {
        UE_LOG(LogMortarPlugin, Warning, TEXT("Mortar Resource Manager - ACtor = %s , POol Size = %d"),
               *Element.Key->GetName(),Element.Value);

        TArray<AActor*>act;
        act.Empty(Element.Value);
        ResourcePool.Add(Element.Key, act);
    }

}

void UMortarResourceManager::SetResourceCount(const TMap<TSubclassOf<class AActor>, int32>& ResourceCount)
{
    PerResourceCount = ResourceCount;
}


//Adds an ACtor To a Tarray 
//If limit exceeds it deletes actors from queue and destory them also
//Then inserts new actor
void UMortarResourceManager::AddToPool(AActor* ActorToAdd,TSubclassOf<AActor> SubClass)
{
    if (PerResourceCount.Contains(SubClass) == false || ResourcePool.Contains(SubClass) == false)
        return;


    {
        FScopeLock ScopeLock(&ArrayProtectorMutex);
        int32 count = PerResourceCount[SubClass];
        if (ResourcePool[SubClass].Num() > count)
        {
            UE_LOG(LogMortarPlugin, Warning, TEXT("Mortar Resource Manager - Warning Pool Size Exceeded. increase resource Size?"));

            int32 j = MaxOneTimeDelete;

            for (auto it = ResourcePool[SubClass].CreateIterator(); it; ++it)
            {
                if (j < 0)
                    break;

                --j;
                AActor* act = *it;
                ResourcePool[SubClass].RemoveAtSwap(it.GetIndex());
                if (act)
                {
                    act->Destroy();
                }


            }

        }
    }
	
    //Add the actor to the correct pool
    ResourcePool[SubClass].Add(ActorToAdd);
}

//Removes Actor From Pool
bool UMortarResourceManager::RemoveFromPool(AActor* ActorToRemove, TSubclassOf<AActor> SubClass)
{
    if (ResourcePool.Contains(SubClass))
    {
        ResourcePool[SubClass].RemoveSwap(ActorToRemove);
        return true;
    }
    return false;
}

                     



