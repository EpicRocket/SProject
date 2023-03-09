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

#if WITH_EDITORONLY_DATA
	virtual void TickActor(float DeltaTime, enum ELevelTick TickType, FActorTickFunction& ThisTickFunction) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return true; }
#endif

public:

	static FName RootName;


protected:

	UPROPERTY()
	EEnvironmentPropType EnvironmentPropType;



};
