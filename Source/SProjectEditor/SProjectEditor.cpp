
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "SProjectEditor"

class FSProjectEditorModule : public FDefaultGameModuleImpl
{
};

IMPLEMENT_MODULE(FSProjectEditorModule, SProjectEditor);

#undef LOCTEXT_NAMESPACE
