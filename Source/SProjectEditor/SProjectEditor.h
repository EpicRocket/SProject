#pragma once

#include "CoreMinimal.h"

class FSProjectEditorModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

