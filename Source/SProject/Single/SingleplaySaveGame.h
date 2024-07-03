
#pragma once

#include "GameFramework/SaveGame.h"
#include "Types/User.h"
#include "Types/Resource.h"
#include "Types/Domain.h"

#include "SingleplaySaveGame.generated.h"


/** 싱글플레이 세이브 게임 */
UCLASS()
class USingleplaySaveGame : public USaveGame
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

/** 싱글플레이 세이브 게임 유틸 클래스 */
UCLASS(BlueprintType, MinimalAPI)
class USingleplaySaveGameContext : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "싱글플레이")
	bool IsExistSaveGame() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "싱글플레이")
	void InitSingleplay();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "싱글플레이")
	bool LoadSingleplay();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "싱글플레이")
	bool SaveSingleplay();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "싱글플레이")
	void DeleteSignleplay();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USingleplaySaveGame* SaveGame;
};
