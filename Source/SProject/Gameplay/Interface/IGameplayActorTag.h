
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "IGameplayActorTag.generated.h"

UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class MY_API UGameplayActorTag : public UInterface
{
	GENERATED_BODY()
};

class MY_API IGameplayActorTag : public IInterface
{
	GENERATED_BODY()

public:
	static const FName PositionTagName;

	UFUNCTION(BlueprintCallable, Category = "�����÷���|����")
	virtual void SetPosition(int32 InPosition);

	UFUNCTION(BlueprintCallable, Category = "�����÷���|����")
	virtual int32 GetPosition() const;

};
