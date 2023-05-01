//Copyright(c) 2021 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MortarActorsObject.generated.h"

/**
 *  This Class Allows to store Tarray which can be used
 *  as a key in Blackboard
 */
UCLASS()
class MORTARPLUGIN_API UMortarActorsObject : public UObject
{
	GENERATED_BODY()

private:

	/**
	 * Member Variable Containing Pointers to actors
	 */
	UPROPERTY()
	TArray<class AActor*> ActorsObject;

public:
	/**
	 *  Assign Tarray of Actors to the internal member
	 *  @param[in] Actors TArray of Actors which will be used later
	 */
	UFUNCTION()
	void SetActorsObject(const TArray<class AActor*>& Actors);

	/**
	*  Returns the assigned Tarray of Actors
	*  @return Actors TArray
	*/
	UFUNCTION()
	TArray<AActor*> GetActorsObject();

};
