#include "TheArchitectAIModule.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;

// Made too many SCVs? Supply depot is a little late
// Made too many barracks (Do I need some form of checking if each command is being executed already?
// - As in, onFrame() is occuring too quickly?
// Do I need to rethink the thread safety/concurrency aspects of what I'm doing?

/////////////////////////////////////////////////
///////// Constructor/Destructor ////////////////
/////////////////////////////////////////////////

TheArchitectAIModule::TheArchitectAIModule()
{
	// Initialize class variables
	initializeClassValues();
}

TheArchitectAIModule::~TheArchitectAIModule()
{
	std::map<UnitType, std::list<Unit>*>::iterator iter;
	for (iter = builderList.begin(); iter != builderList.end(); ++iter) {
		delete iter->second;
	}
}

/////////////////////////////////////////////////
///////// Initialization Methods ////////////////
/////////////////////////////////////////////////

// Initialize all class variables for our AI module
void TheArchitectAIModule::initializeClassValues()
{
	// Create our list of builders
	initializeBuilderTypes();

	// Make our initial command queue
	initializeQueue();
}

void TheArchitectAIModule::initializeBuilderTypes()
{
	// Gather list of all builder types and initialize our map to our builders
	UnitType scvType = UnitTypes::Terran_SCV;
	UnitType ccType = UnitTypes::Terran_Command_Center;
	UnitType barracksType = UnitTypes::Terran_Barracks;
	UnitType factoryType = UnitTypes::Terran_Factory;
	UnitType starportType = UnitTypes::Terran_Starport;

	static int numBuilders_ = 5;
	static UnitType builderTypes[5] = { scvType, ccType, barracksType, factoryType, starportType };

	builderList = std::map<UnitType, std::list<Unit>*>();
	for (int i = 0; i < numBuilders_; ++i) {
		UnitType type = builderTypes[i];
		builderList[type] = new std::list<Unit>();
	}
}

void TheArchitectAIModule::initializeQueue()
{
	
	commandQueue = std::priority_queue<BuildCommand, std::vector<BuildCommand>, BuildCommandCompare>();
	UnitType scvType = UnitTypes::Terran_SCV;
	UnitType supplyDepotType = UnitTypes::Terran_Supply_Depot;
	UnitType barracksType = UnitTypes::Terran_Barracks;
	commandQueue.push(BuildCommand(scvType, 10));
	commandQueue.push(BuildCommand(scvType, 10));
	commandQueue.push(BuildCommand(scvType, 10));
	commandQueue.push(BuildCommand(scvType, 10));
	commandQueue.push(BuildCommand(scvType, 10));
	commandQueue.push(BuildCommand(supplyDepotType, 9));
	commandQueue.push(BuildCommand(scvType, 8));
	commandQueue.push(BuildCommand(scvType, 8));
	commandQueue.push(BuildCommand(scvType, 8));
	commandQueue.push(BuildCommand(scvType, 8));
	commandQueue.push(BuildCommand(barracksType, 7));
}

/////////////////////////////////////////////////
///////// BWAPI Callbacks ///////////////////////
/////////////////////////////////////////////////

void TheArchitectAIModule::onStart()
{
	// Hello World!
	Broodwar->sendText("Hello world!");

	

	// Print the map name.
	// BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
	Broodwar << "The map is " << Broodwar->mapName() << "!" << std::endl;

	// Enable the UserInput flag, which allows us to control the bot and type messages.
	Broodwar->enableFlag(Flag::UserInput);

	// Uncomment the following line and the bot will know about everything through the fog of war (cheat).
	//Broodwar->enableFlag(Flag::CompleteMapInformation);

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM (Actions Per Minute).
	Broodwar->setCommandOptimizationLevel(2);

	// Check if this is a replay
	if (Broodwar->isReplay())
	{

		// Announce the players in the replay
		Broodwar << "The following players are in this replay:" << std::endl;

		// Iterate all the players in the game using a std:: iterator
		Playerset players = Broodwar->getPlayers();
		for (auto p : players)
		{
			// Only print the player if they are not an observer
			if (!p->isObserver())
				Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
		}

	}
	else // if this is not a replay
	{
		// Retrieve you and your enemy's races. enemy() will just return the first enemy.
		// If you wish to deal with multiple enemies then you must use enemies().
		if (Broodwar->enemy()) // First make sure there is an enemy
			Broodwar << "The matchup is " << Broodwar->self()->getRace() << " vs " << Broodwar->enemy()->getRace() << std::endl;
	}



}

void TheArchitectAIModule::onEnd(bool isWinner)
{
	// Called when the game ends
	if (isWinner)
	{
		// Log your win here!
	}
}

void TheArchitectAIModule::onFrame()
{
	// Called once every game frame

	// Display the game frame rate as text in the upper left area of the screen
	Broodwar->drawTextScreen(200, 0, "FPS: %d", Broodwar->getFPS());
	Broodwar->drawTextScreen(200, 20, "Average FPS: %f", Broodwar->getAverageFPS());

	// Return if the game is a replay or is paused
	if (Broodwar->isReplay() || Broodwar->isPaused() || !Broodwar->self())
		return;

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if (Broodwar->getFrameCount() % Broodwar->getLatencyFrames() != 0)
		return;



	// Examine the top item in our command queue. If we can build it, do so!
	BuildCommand top = commandQueue.top();
	int mins = Broodwar->self()->minerals();
	int gas = Broodwar->self()->gas();
	int supplyRoom = Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed();
	if (top.canCompleteOrder(mins, gas, supplyRoom)) {
		if (executeCommand(top)) {
			commandQueue.pop();
		}
	}

	// Iterate through all the units that we own
	for (auto &u : Broodwar->self()->getUnits())
	{
		// Ignore the unit if it no longer exists
		// Make sure to include this block when handling any Unit pointer!
		if (!u->exists())
			continue;

		// Ignore the unit if it has one of the following status ailments
		if (u->isLockedDown() || u->isMaelstrommed() || u->isStasised())
			continue;

		// Ignore the unit if it is in one of the following states
		if (u->isLoaded() || !u->isPowered() || u->isStuck())
			continue;

		// Ignore the unit if it is incomplete or busy constructing
		if (!u->isCompleted() || u->isConstructing())
			continue;


		// Finally make the unit do some stuff!


		// If the unit is a worker unit
		if (u->getType().isWorker())
		{
			// if our worker is idle
			if (u->isIdle())
			{
				// Order workers carrying a resource to return them to the center,
				// otherwise find a mineral patch to harvest.
				if (u->isCarryingGas() || u->isCarryingMinerals())
				{
					u->returnCargo();
				}
				else if (!u->getPowerUp())  // The worker cannot harvest anything if it
				{                             // is carrying a powerup such as a flag


					// Harvest from the nearest mineral patch or gas refinery
					if (!u->gather(u->getClosestUnit(IsMineralField || IsRefinery)))
					{
						// If the call fails, then print the last error message
						Broodwar << Broodwar->getLastError() << std::endl;
					}

				} // closure: has no powerup
			} // closure: if idle

		}
		

		

	} // closure: unit iterator
}


void TheArchitectAIModule::onSendText(std::string text)
{

	// Send the text to the game if it is not being processed.
	Broodwar->sendText("%s", text.c_str());


	// Make sure to use %s and pass the text as a parameter,
	// otherwise you may run into problems when you use the %(percent) character!

}

void TheArchitectAIModule::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void TheArchitectAIModule::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void TheArchitectAIModule::onNukeDetect(BWAPI::Position target)
{

	// Check if the target is a valid position
	if (target)
	{
		// if so, print the location of the nuclear strike target
		Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else
	{
		// Otherwise, ask other players where the nuke is!
		Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using Broodwar->getNukeDots()!
}

void TheArchitectAIModule::onUnitDiscover(BWAPI::Unit unit)
{
}

void TheArchitectAIModule::onUnitEvade(BWAPI::Unit unit)
{
}

void TheArchitectAIModule::onUnitShow(BWAPI::Unit unit)
{
}

void TheArchitectAIModule::onUnitHide(BWAPI::Unit unit)
{
}

void TheArchitectAIModule::onUnitCreate(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}
}

void TheArchitectAIModule::onUnitDestroy(BWAPI::Unit unit)
{
}

void TheArchitectAIModule::onUnitMorph(BWAPI::Unit unit)
{
	if (Broodwar->isReplay())
	{
		// if we are in a replay, then we will print out the build order of the structures
		if (unit->getType().isBuilding() && !unit->getPlayer()->isNeutral())
		{
			int seconds = Broodwar->getFrameCount() / 24;
			int minutes = seconds / 60;
			seconds %= 60;
			Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}
}

void TheArchitectAIModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void TheArchitectAIModule::onSaveGame(std::string gameName)
{
	Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void TheArchitectAIModule::onUnitComplete(BWAPI::Unit unit)
{
	// If the finished unit is a "builder unit" SCV/Barracks/CC/Factory/Starports,
	// add it to a list of of things that make stuff.
	std::map<UnitType, std::list<Unit>*>::iterator iter;
	UnitType typeForUnit = unit->getType();
	iter = builderList.find(typeForUnit);
	if (iter != builderList.end()) {
		std::list<Unit>* listForType = iter->second;
		listForType->push_back(unit);
	}
}



// Exectures a build command, either trying to make a unit or a structure
// Assumes that we already have enough minerals/gas/supply for this command,
// but could still fail if there are no idle builder. Thus it returns true
// if it succeeds, false otherwise.
bool TheArchitectAIModule::executeCommand(BuildCommand command)
{
	UnitType type = command.getUnitType();
	if (type.isBuilding()) {
		UnitType buildType = type.whatBuilds().first; // Should be SCV
		std::list<Unit> buildersOfThisType = *builderList[buildType];
		for (Unit& u : buildersOfThisType) {
			if (u->exists()) {
				if (u->isIdle() || u->isGatheringMinerals()) {
					TilePosition targetBuildLocation = Broodwar->getBuildLocation(type, u->getTilePosition());
					if (targetBuildLocation)
					{
						Broodwar->sendText("Starting to construct building!");
						// Order the builder to construct the supply structure
						u->build(type, targetBuildLocation);
						return true;
					}
				} // End isIdle/Gathering
			} // End exists

		} // End unit loop
	}
	else {
		// Assume this is a unit
		UnitType buildType = type.whatBuilds().first;
		std::list<Unit> buildersOfThisType = *builderList[buildType];
		for (Unit& u : buildersOfThisType) {
			if (u->exists()) {
				if (u->isIdle()) {
					if (u->train(type)) {
						return true;
					}
					else {
						// If that fails, draw the error at the location so that you can visibly see what went wrong!
						// However, drawing the error once will only appear for a single frame
						// so create an event that keeps it on the screen for some frames
						Position pos = u->getPosition();
						Error lastErr = Broodwar->getLastError();
						Broodwar->registerEvent([pos, lastErr](Game*){ Broodwar->drawTextMap(pos, "%c%s", Text::White, lastErr.c_str()); },   // action
							nullptr,    // condition
							Broodwar->getLatencyFrames());  // frames to run
					}
				} // End isIdle
			} // End exists
			
		} // End unit loop

	} // End else
	// If we got this far, then we didn't build anything!
	return false;
}