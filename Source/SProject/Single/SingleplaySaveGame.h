﻿
#pragma once

#include "GameFramework/SaveGame.h"
#include "Templates/SharedPointer.h"
#include "Types/DocumentTypes.h"

#include "SingleplaySaveGame.generated.h"

struct FFetchDocument;

/** 싱글플레이 세이브 게임 */
UCLASS()
class USingleplaySaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUserAccount UserAccount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FUserDocument UserDocument;
};

/** 싱글플레이 세이브 게임 유틸 클래스 */
UCLASS(BlueprintType, MinimalAPI)
class USingleplaySaveGameContext : public UObject
{
	GENERATED_BODY()

public:
	static FString Token;

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

	FFetchDocument FetchDocument();

private:
	TSharedRef<FUserDocument> GetUserDocument() const;

	void OnUpdateUserDocument();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString SlotName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USingleplaySaveGame* SaveGame;

private:
	mutable TSharedPtr<FUserDocument> UserDocumentPtr;
};
