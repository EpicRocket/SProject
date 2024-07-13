
#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "UserDocumentMiddleware.generated.h"

struct FFetchDocument;


UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class SPROJECT_API UUserDocumentMiddleware : public UInterface
{
	GENERATED_BODY()
};

class SPROJECT_API IUserDocumentMiddleware
{
	GENERATED_BODY()

public:
	// 변경 사항이 있는 FetchDocument를 파라미터로 받아 실제 내부 데이터를 수정하는 함수
	virtual void ApplyUserDocumentChanges(const TSharedRef<FFetchDocument> FetchDocument) = 0;

    // 인터페이스를 가진 객체들이 ApplyUserDocumentChanges를 모두 호출한 뒤 호출되는 함수
	virtual void FinalizeUserDocumentUpdate(const TSharedRef<FFetchDocument> FetchDocument) {}
};
