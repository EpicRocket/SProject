
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "UserDocumentMiddleware.generated.h"

struct FFetchDocument;


UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class MY_API UUserDocumentMiddleware : public UInterface
{
	GENERATED_BODY()
};

class MY_API IUserDocumentMiddleware
{
	GENERATED_BODY()

public:
	virtual void ApplyUserDocumentChanges(const TSharedRef<FFetchDocument> FetchDocument) = 0;

	virtual void FinalizeUserDocumentUpdate(const TSharedRef<FFetchDocument> FetchDocument) {}
};
