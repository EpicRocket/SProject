//Copyright(c) 2021 FinalRockstar All Rights Reserved

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MortarTeamComponent.generated.h"

/**
 * Event Called On Team Change
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FTeamChangeEvent);

/**
* Copy of ETeamAttribute 
* Types of Attitudes two players can have towards each other
*/
UENUM(BlueprintType)
namespace EMortarAgentAttitude
{
	enum Type
	{
		Friendly, ///< Both the Actors Are Friendly to each Other :)
		Neutral, ///< Both the Actors Are Neutral to each Other :|
		Hostile ///< Both the Actors Are Enemies :(
	};
}

/**
 * Structure Containing Team Attributes
 */
USTRUCT(Blueprintable)
struct FMortarTeamProperties
{
	GENERATED_USTRUCT_BODY()

	/**
	 * Team ID of a player from 0 -255 . 0 is a default/Neutral state
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team|Setup")
	uint8  TeamID = 0;

	/**
	 * Team Name Better to have unique name
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team|Setup")
	FString  TeamName = "Neutral";

	/**
	 * Color of each team
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Team|Setup")
	FLinearColor  TeamColor;
};

/**
 *  Generic Actor Component That can be assigned to any Actor
 *  Each Actor then can have a team id,name and color and also team
 */
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MORTARPLUGIN_API UMortarTeamComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMortarTeamComponent();

	/**
	 * Sets Information For a team
	 * @param[in] DesiredTeam Team Properties to Set
	 */
	UFUNCTION(BlueprintCallable, Category = "Team|Setup")
	void SetTeamInfo(const FMortarTeamProperties& DesiredTeam);

	/**
	 * Returns Current Team Properties
	 * @return Structure containing all team related info
	 */
	UFUNCTION(BlueprintPure, Category = "Team|Setup")
	FMortarTeamProperties  GetTeamInfo() const;

	/**
	 * Sets Default Information For a team which can be used later
	 * @param[in] DefaultTeamProperties Default Team Properties to Set
	 */
	UFUNCTION(BlueprintCallable, Category = "Team|Setup")
	void SetDefaultTeamInfo(const FMortarTeamProperties& DefaultTeamProperties);

	/**
	 * Returns Default Team Properties
	 * @return Structure containing all team related info
	 */
	UFUNCTION(BlueprintPure, Category = "Team|Setup")
	FMortarTeamProperties  GetDefaultTeamInfo() const;

	/**
	 * Resets Team Information to Default
	 */
	UFUNCTION(BlueprintCallable, Category = "Team|Setup")
	void ResetTeamInfo();

	/**
	 * Static Function to Find Attitude of Two players and returns the attitude
	 * @param[in] ATeamID - Team ID of Player A
	 * @param[in] BTeamID - Team ID of Player B
	 * @return Returns Attitude towards each other as enum => Friendly,Neutral,Hostile
	 */
	UFUNCTION(BlueprintCallable, Category = "Team|Setup")
	static  EMortarAgentAttitude::Type  GetAttitude(const uint8& ATeamID, const uint8& BTeamID);

	/**
	 * Tries to switches the team for the player. This will be possible only when
	 * Current Team ID is neutral
	 * @param[in] DesiredTeam Team Properties to Set
	 */
	UFUNCTION(BlueprintCallable, Category = "Team|Setup")
	bool DoSwitchTeam(const FMortarTeamProperties& DesiredTeam);

	/**
	 *  This will be the neutral team id.
	 */
	static uint8 NeutralTeamID;

	/**
	 *   Team Change Event That Will be Broadcasted as soon as Team changes from Neutral to Desired Team in SwitchTeam
	 */
	UPROPERTY(BlueprintAssignable)
	FTeamChangeEvent OnTeamChange;

private:

	/**
	 * Current  Team ID - This is The Team information for Current Spawn.
	 * TeamID - Should Be unique and other than 0 which is neutral. Different TeamID means 
	 * They are enemies.Same TeamID means they are friends
	 * TeamName - Each Team Can have a name which can be used in blueprint for different purposes
	 * TeamColor - Color For each Team
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Team", meta = (TitleProperty = "TeamName"))
	FMortarTeamProperties  TeamInfo;

	/**
	 * Default  Team ID - This is The Team information used after every Respawn. So if a player of Team A dies
	 * then it will switch back to the default Team after respawning
	 * TeamID - Should Be unique and other than 0 which is neutral. Different TeamID means
	 * They are enemies.Same TeamID means they are friends
	 * TeamName - Each Team Can have a name which can be used in blueprint for different purposes
	 * TeamColor - Color For each Team
	 */
	UPROPERTY(EditAnywhere, Category = "Mortar|Team", meta = (TitleProperty = "TeamName"))
	FMortarTeamProperties  DefaultTeamInfo;


};
