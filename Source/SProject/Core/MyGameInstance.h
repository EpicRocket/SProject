
#pragma once

#include "Core/GGameInstance.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "MyGameInstance.generated.h"

UCLASS()
class MY_API UMyGameInstance : public UGGameInstance
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void LoadStartupTable();
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnLoadStartupTable();
	virtual void OnLoadStartupTable();

public:
	UFUNCTION(BlueprintCallable)
	void LoadTable();
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void K2_OnTableLoad();
	virtual void OnTableLoad();

};

UCLASS()
class MY_API UMyGameInstanceHelper : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "寃뚯엫濡쒕뱶", meta = (WorldContext = "WorldContextObject"))
	static void LoadStartupTable(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category = "寃뚯엫濡쒕뱶", meta = (WorldContext = "WorldContextObject"))
	static void LoadTable(UObject* WorldContextObject);
};
