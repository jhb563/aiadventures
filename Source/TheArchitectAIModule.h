#pragma once
#include <BWAPI.h>
#include "buildcommand.h"
#include <queue>

// Remember not to use "Broodwar" in any global class constructor!

class TheArchitectAIModule : public BWAPI::AIModule
{
public:
	// Virtual functions for callbacks, leave these as they are.
	virtual void onStart();
	virtual void onEnd(bool isWinner);
	virtual void onFrame();
	virtual void onSendText(std::string text);
	virtual void onReceiveText(BWAPI::Player player, std::string text);
	virtual void onPlayerLeft(BWAPI::Player player);
	virtual void onNukeDetect(BWAPI::Position target);
	virtual void onUnitDiscover(BWAPI::Unit unit);
	virtual void onUnitEvade(BWAPI::Unit unit);
	virtual void onUnitShow(BWAPI::Unit unit);
	virtual void onUnitHide(BWAPI::Unit unit);
	virtual void onUnitCreate(BWAPI::Unit unit);
	virtual void onUnitDestroy(BWAPI::Unit unit);
	virtual void onUnitMorph(BWAPI::Unit unit);
	virtual void onUnitRenegade(BWAPI::Unit unit);
	virtual void onSaveGame(std::string gameName);
	virtual void onUnitComplete(BWAPI::Unit unit); 
	// Everything below this line is safe to modify.

	// Need the define default constructor and destructor
	TheArchitectAIModule();
	~TheArchitectAIModule();

private:

	// Disable Copy constructor and assignment operator
	TheArchitectAIModule(const TheArchitectAIModule& other);
	TheArchitectAIModule* operator=(const TheArchitectAIModule& other);

	struct BuildCommandCompare {
		bool operator() (const BuildCommand& lhs, const BuildCommand& rhs) {
			return lhs.getPriority() < rhs.getPriority();
		}
	};

	// Order Priority Queue
	std::priority_queue<BuildCommand, std::vector<BuildCommand>, BuildCommandCompare> commandQueue;

	

	// Builder Map
	// Maps from the type of unit to a list of the units we have of that type
	std::map<BWAPI::UnitType, std::list<BWAPI::Unit>*> builderList;

	// Method to be called to initialize all class values
	void initializeClassValues();

	// Method to initialize our list of builders
	void initializeBuilderTypes();

	// Method to be called to initialize our command queue at the start of the game.
	void initializeQueue();

	// Method to be called to execute a build command
	// Input:
	//	The command to be exectured
	//
	// Output:
	//	Whether or not the command was successful
	bool executeCommand(BuildCommand command);

	

	


};
