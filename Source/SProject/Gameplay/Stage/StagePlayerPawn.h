
#pragma once

#include "GameFramework/Pawn.h"

#include "StagePlayerPawn.generated.h"

class AController;
class AStageLevel;

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class EStagePlayerFlag : uint8
{
	None,
	Build,
};
ENUM_CLASS_FLAGS(EStagePlayerFlag);

UCLASS(Abstract, BlueprintType, Blueprintable, hideCategories = (Navigation), ClassGroup = "Stage")
class MY_API AStagePlayerPawn : public APawn
{
	GENERATED_BODY()

public:
	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

private:
	void OnUpdateState();

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
	TWeakObjectPtr<AStageLevel> CurrentLevel;

private:
	UPROPERTY(Transient, meta = (Bitmask, BitmaskEnum = "/Script/SProject.EStagePlayerFlag"))
	EStagePlayerFlag PlayerFlags = EStagePlayerFlag::None;
};
