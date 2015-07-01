#pragma once
#include <BWAPI.h>
#include <queue>
#include <resourcemanager.h>
#include <buildmanager.h>
#include <productionmanager.h>

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

	// Need to define default constructor and destructor
	TheArchitectAIModule();
	~TheArchitectAIModule();

private:

	// Disable Copy constructor and assignment operator
	TheArchitectAIModule(const TheArchitectAIModule& other);
	TheArchitectAIModule* operator=(const TheArchitectAIModule& other);

	// We have a pointer to the ResourceManager, BuildManager, and the
	// ProductionManager
	ResourceManager* resourceManager_;
	BuildManager* buildManager_;
	ProductionManager* productionManager_;
};
