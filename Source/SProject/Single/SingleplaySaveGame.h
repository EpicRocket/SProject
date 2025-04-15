
#pragma once

#include "GameFramework/SaveGame.h"
#include "Templates/SharedPointer.h"
#include "Types/DocumentTypes.h"

#include "SingleplaySaveGame.generated.h"

struct FFetchDocument;

/** ?깃??뚮젅???몄씠釉?寃뚯엫 */
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

/** ?깃??뚮젅???몄씠釉?寃뚯엫 ?좏떥 ?대옒??*/
UCLASS(BlueprintType, MinimalAPI)
class USingleplaySaveGameContext : public UObject
{
	GENERATED_BODY()

public:
	static FString Token;

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "?깃??뚮젅??)
	bool IsExistSaveGame() const;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "?깃??뚮젅??)
	void InitSingleplay();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "?깃??뚮젅??)
	bool LoadSingleplay();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "?깃??뚮젅??)
	bool SaveSingleplay();

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "?깃??뚮젅??)
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
