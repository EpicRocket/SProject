// Copyright 2022 Silvan Teufel / Teufel-Engineering.com All Rights Reserved.

#include "Controller/Input/GameplayTags.h"
#include "GameplayTagsManager.h"
#include "Engine/EngineTypes.h"

FGameplayTags FGameplayTags::GameplayTags;

void FGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& GameplayTagsManager = UGameplayTagsManager::Get();

	GameplayTags.AddAllTags(GameplayTagsManager);

	GameplayTagsManager.DoneAddingNativeTags();
}

void FGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	AddTag(InputTag_LeftClick_Pressed, "InputTag.LeftClick_Pressed", "LeftClick_Pressed");
	AddTag(InputTag_LeftClick_Released, "InputTag.LeftClick_Released", "LeftClick_Released");
	
	AddTag(InputTag_RightClick_Pressed, "InputTag.RightClick_Pressed", "RightClick_Pressed");
	
	AddTag(InputTag_Ctrl_Pressed, "InputTag.Ctrl_Pressed", "Ctrl_Pressed");
	AddTag(InputTag_Ctrl_Released, "InputTag.Ctrl_Released", "Ctrl_Released");

	AddTag(InputTag_Tab_Pressed, "InputTag.Tab_Pressed", "Tab_Pressed");
	AddTag(InputTag_Tab_Released, "InputTag.Tab_Released", "Tab_Released");
	
	AddTag(InputTag_Alt_Pressed, "InputTag.Alt_Pressed", "Alt_Pressed");
	AddTag(InputTag_Alt_Released, "InputTag.Alt_Released", "Alt_Released");
	
	AddTag(InputTag_Shift_Pressed, "InputTag.Shift_Pressed", "Shift_Pressed");
	AddTag(InputTag_Shift_Released, "InputTag.Shift_Released", "Shift_Released");

	AddTag(InputTag_Space_Pressed, "InputTag.Space_Pressed", "Space_Pressed");
	AddTag(InputTag_Space_Released, "InputTag.Space_Released", "Space_Released");
	
	AddTag(InputTag_Q_Pressed, "InputTag.Q_Pressed", "Q_Pressed");
	
	AddTag(InputTag_W_Pressed, "InputTag.W_Pressed", "W_Pressed");
	AddTag(InputTag_A_Pressed, "InputTag.A_Pressed", "A_Pressed");
	AddTag(InputTag_S_Pressed, "InputTag.S_Pressed", "S_Pressed");
	AddTag(InputTag_D_Pressed, "InputTag.D_Pressed", "D_Pressed");

	AddTag(InputTag_W_Released, "InputTag.W_Released", "W_Released");
	AddTag(InputTag_A_Released, "InputTag.A_Released", "A_Released");
	AddTag(InputTag_S_Released, "InputTag.S_Released", "S_Released");
	AddTag(InputTag_D_Released, "InputTag.D_Released", "D_Released");

	AddTag(InputTag_Y_Pressed, "InputTag.Y_Pressed", "Y_Pressed");
	AddTag(InputTag_X_Pressed, "InputTag.X_Pressed", "X_Pressed");

	AddTag(InputTag_Y_Released, "InputTag.Y_Released", "Y_Released");
	AddTag(InputTag_X_Released, "InputTag.X_Released", "X_Released");
	
	AddTag(InputTag_E_Pressed, "InputTag.E_Pressed", "E_Pressed");
	AddTag(InputTag_R_Pressed, "InputTag.R_Pressed", "R_Pressed");

	AddTag(InputTag_F_Pressed, "InputTag.F_Pressed", "F_Pressed");

	AddTag(InputTag_C_Pressed, "InputTag.C_Pressed", "C_Pressed");
	
	AddTag(InputTag_G_Pressed, "InputTag.G_Pressed", "G_Pressed");
	
	AddTag(InputTag_T_Pressed, "InputTag.T_Pressed", "T_Pressed");

}

void FGameplayTags::AddTag(FGameplayTag& OutTag, const ANSICHAR* TagName, const ANSICHAR* TagComment)
{
	OutTag = UGameplayTagsManager::Get().AddNativeGameplayTag(FName(TagName), FString(TEXT("(Native) ")) + FString(TagComment));
}