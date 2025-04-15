
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
	// 蹂寃??ы빆???덈뒗 FetchDocument瑜??뚮씪誘명꽣濡?諛쏆븘 ?ㅼ젣 ?대? ?곗씠?곕? ?섏젙?섎뒗 ?⑥닔
	virtual void ApplyUserDocumentChanges(const TSharedRef<FFetchDocument> FetchDocument) = 0;

    // ?명꽣?섏씠?ㅻ? 媛吏?媛앹껜?ㅼ씠 ApplyUserDocumentChanges瑜?紐⑤몢 ?몄텧?????몄텧?섎뒗 ?⑥닔
	virtual void FinalizeUserDocumentUpdate(const TSharedRef<FFetchDocument> FetchDocument) {}
};
