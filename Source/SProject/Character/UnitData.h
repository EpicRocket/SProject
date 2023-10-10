
#pragma once

#include "Engine/DataAsset.h"
#include "Templates/SubclassOf.h"

#include "UnitData.generated.h"

/**
 *	Non-mutable data asset that contains properties used to define a pawn.
 */
UCLASS(BlueprintType, Const, Meta = (DisplayName = "Unit Pawn Data", ShortTooltip = "Data asset used to define a Pawn."))
class UUnitData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	// Class to instantiate for this pawn (should usually derive from ALyraPawn or ALyraCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit|Pawn")
	TSubclassOf<class APawn> PawnClass;

	// // Ability sets to grant to this pawn's ability system.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit|Abilities")
	TArray<TObjectPtr<class UUnitAbilitySet>> AbilitySets;

	// // What mapping of ability tags to use for actions taking by this pawn
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Abilities")
	// TObjectPtr<ULyraAbilityTagRelationshipMapping> TagRelationshipMapping;

	// Input configuration used by player controlled pawns to create input mappings and bind input actions.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Unit|Input")
	TObjectPtr<struct FSuperInputAction> InputConfig;

	// // Default camera mode used by player controlled pawns.
	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Camera")
	// TSubclassOf<ULyraCameraMode> DefaultCameraMode;
};
