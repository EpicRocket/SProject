#include "SProjectEditor.h"

#include "Modules/ModuleManager.h"
#include "ContentBrowserModule.h"
#include "IContentBrowserSingleton.h"

IMPLEMENT_MODULE(FSProjectEditorModule, SProjectEditor);

void FSProjectEditorModule::StartupModule()
{
	//IContentBrowserSingleton& ContentBrowser = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();
	

}

void FSProjectEditorModule::ShutdownModule()
{
}
