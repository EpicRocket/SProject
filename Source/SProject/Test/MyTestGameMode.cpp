
#include "MyTestGameMode.h"
// include Engine
// include Project
#include "Core/MyGameInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(MyTestGameMode)

void AMyTestGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	auto MyGameInstance = Cast<UMyGameInstance>(GetGameInstance());
	if (MyGameInstance)
	{
		MyGameInstance->LoadStartupTable();
		MyGameInstance->LoadTable();
	}
}
