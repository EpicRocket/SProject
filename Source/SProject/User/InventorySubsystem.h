
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "Templates/SharedPointer.h"
#include "UserDocumentMiddleware.h"

#include "InventorySubsystem.generated.h"

struct FFetchDocument;
struct FItem;

UCLASS()
class MY_API UInventorySubsystem : public ULocalPlayerSubsystem, public IUserDocumentMiddleware
{
	GENERATED_BODY()
	
public:
	static UInventorySubsystem* Get(const ULocalPlayer* InLocalPlayer);

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// IUserDocumentMiddleware
	virtual void ApplyUserDocumentChanges(const TSharedRef<FFetchDocument> FetchDocument) override;
	// ~IUserDocumentMiddleware

	UFUNCTION(BlueprintCallable)
	int64 GetGold() const;

	UFUNCTION(BlueprintCallable)
	int32 GetCash() const;

	UFUNCTION(BlueprintCallable)
	FItem Get(int32 Key) const;

private:
	TSharedRef<FItem> GetItem(int32 Key) const;

private:
	int64 Gold = 0;

	int32 Cash = 0;

	mutable TMap<int32, TSharedPtr<FItem>> Items;
};
