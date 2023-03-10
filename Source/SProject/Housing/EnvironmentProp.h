#pragma once

#include "GameFramework/Actor.h"
#include "EnvironmentProp.generated.h"


enum class EEnvironmentPropType : uint8;

UCLASS()
class SPROJECT_API AEnvironmentProp : public AActor
{
	GENERATED_BODY()

public:

	AEnvironmentProp();

public:

	static FName RootName;


protected:

	UPROPERTY()
	EEnvironmentPropType EnvironmentPropType;

};
