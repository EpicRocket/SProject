
#pragma once

#include "GameFramework/SaveGame.h"
#include "Templates/SharedPointer.h"
#include "Types/DocumentTypes.h"

#include "SingleplaySaveGame.generated.h"

struct FFetchDocument;

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

UCLASS(BlueprintType, MinimalAPI)
class USingleplaySaveGameContext : public UObject
{
	GENERATED_BODY()

public:
	static FString Token;

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	bool IsExistSaveGame() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	void InitSingleplay();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	bool LoadSingleplay();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
	bool SaveSingleplay();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic)
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
