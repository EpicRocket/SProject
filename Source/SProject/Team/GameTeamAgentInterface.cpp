
#include "GameTeamAgentInterface.h"
#include "SProject.h"
#include "UObject/ScriptInterface.h"

void IGameTeamAgentInterface::ConditionalBroadcastTeamChanged(TScriptInterface<IGameTeamAgentInterface> This, FGenericTeamId OldTeamID, FGenericTeamId NewTeamID)
{
    if (OldTeamID != NewTeamID)
	{
		const int32 OldTeamIndex = GenericTeamIdToInteger(OldTeamID); 
		const int32 NewTeamIndex = GenericTeamIdToInteger(NewTeamID);

		UObject* ThisObj = This.GetObject();
		UE_LOG(Project, Verbose, TEXT("%s assigned team %d"), *GetPathNameSafe(ThisObj), NewTeamIndex);

		This.GetInterface()->GetTeamChangedDelegateChecked().Broadcast(ThisObj, OldTeamIndex, NewTeamIndex);
	}
}
