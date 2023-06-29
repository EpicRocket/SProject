// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/InheritableComponentHandler.h"
#include "Engine/SCS_Node.h"
#include "Engine/SimpleConstructionScript.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class SPROJECT_API USFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Sort")
	static void SortUObjectMapByIntegerKey(UPARAM(ref) TMap<int, UObject*>& Map);

	template<class T, class AllocatorType>
	static void GetComponents(const TSubclassOf<AActor>& ActorClass, TArray<T*, AllocatorType>& OutComponents)
	{		
		OutComponents.Reset();
		
		//Blueprint Classes
		{
			const UBlueprintGeneratedClass* CurrentBGClass = Cast<UBlueprintGeneratedClass>(ActorClass);
		
			while (CurrentBGClass)
			{
				//UInheritableComponentHandler
				if (const UInheritableComponentHandler* Handler = CurrentBGClass->InheritableComponentHandler) {
					TArray<UActorComponent*> Templates;
					Handler->GetAllTemplates(Templates);
					for (auto ComponentTemplate : Templates)
					{
						if(T* TargetComponent = Cast<T>(ComponentTemplate))
						{
							OutComponents.AddUnique(TargetComponent);
						}
					}
				}				

				//USimpleConstructionScript
				if (const USimpleConstructionScript* SCS = CurrentBGClass->SimpleConstructionScript) {
					for (const auto Node : SCS->GetAllNodes())
					{
						if(T* TargetComponent = Cast<T>(Node->ComponentTemplate))
						{
							OutComponents.AddUnique(TargetComponent);							
						}
					}
				}
				CurrentBGClass = Cast<UBlueprintGeneratedClass>(CurrentBGClass->GetSuperClass());
			}
		}
		
		//C++ Classes
		{
			TSubclassOf<UObject> CurrentBPClass = ActorClass;

			while (CurrentBPClass)
			{
				TArray<UObject*> CurrentSubobjects;
				CurrentBPClass->GetDefaultObjectSubobjects(CurrentSubobjects);
				for (auto Object : CurrentSubobjects)
				{
					if(T* TargetComponent = Cast<T>(Object))
					{
						OutComponents.AddUnique(TargetComponent);
					}					
				}
				CurrentBPClass = CurrentBPClass->GetSuperClass();
			}
		}
	}
};
