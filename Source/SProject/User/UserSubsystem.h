
#pragma once

#include "Subsystems/LocalPlayerSubsystem.h"
#include "UserDocumentMiddleware.h"

#include "UserSubsystem.generated.h"


struct FFetchDocument;
struct FUserInfo;
class ULocalPlayer;

UCLASS()
class MY_API UUserSubsystem : public ULocalPlayerSubsystem, public IUserDocumentMiddleware
{
	GENERATED_BODY()
	
public:
	static UUserSubsystem* Get(const ULocalPlayer* InLocalPlayer);

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	// IUserDocumentMiddleware
	virtual void ApplyUserDocumentChanges(const TSharedRef<FFetchDocument> FetchDocument) override;
	// ~IUserDocumentMiddleware

	UFUNCTION(BlueprintCallable)
	FString GetUserName() const;

	UFUNCTION(BlueprintCallable)
	FString GetUserTag() const;

	UFUNCTION(BlueprintCallable)
	int32 GetLevel() const;

private:
	TSharedRef<FUserInfo> GetUserInfo() const;

private:
	mutable TSharedPtr<FUserInfo> UserInfo;
};
