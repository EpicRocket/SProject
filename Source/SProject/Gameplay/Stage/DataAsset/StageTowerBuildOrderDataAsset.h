
#pragma once

#include "Engine/DataAsset.h"

#include "StageTowerBuildOrderDataAsset.generated.h"

enum class EStageTowerType : uint8;

USTRUCT(BlueprintType)
struct FStageTowerBuildOrderData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EStageTowerType TowerType = static_cast<EStageTowerType>(0);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 Kind = 0;
};

UCLASS()
class MY_API UStageTowerBuildOrderDataAsset : public UDataAsset
{
    GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FStageTowerBuildOrderData> TowerBuildOrders;
};
