
#pragma once

#include "GameFramework/SaveGame.h"
#include "Types/User.h"
#include "Types/Resource.h"
#include "Types/Domain.h"

#include "SingleplaySaveGame.generated.h"


/** 싱글플레이 세이브 게임 */
UCLASS()
class SPROJECT_API USingleplaySaveGame : public USaveGame
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUserInfo UserInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int64 Gold = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Cash = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FItem> Items;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FDomainBuilding> DomainBuildings;
};
