#include "TheArchitectAIModule.h"
#include <iostream>

using namespace BWAPI;
using namespace Filter;


/////////////////////////////////////////////////
///////// Constructor/Destructor ////////////////
/////////////////////////////////////////////////

TheArchitectAIModule::TheArchitectAIModule()
{
	
	productionManager_ = new ProductionManager();
	buildManager_ = new BuildManager();
	resourceManager_ = new ResourceManager(buildManager_, productionManager_);
	// Initialize the Strategy Manager
	strategyManager_ = new StrategyManager();
}

TheArchitectAIModule::~TheArchitectAIModule()
{
	delete resourceManager_;
	delete productionManager_;
	delete buildManager_;
	delete strategyManager_;
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

	// See if it is time for a new order from the strategy manager, and enqueue any
	// returned orders.
	int currentFrame = Broodwar->getFrameCount();
	
	std::list<UnitType>* nextOrders = strategyManager_->getOrdersForTime(currentFrame);
	if (nextOrders->size() > 0) {
		// Broodwar->sendText("Orders to enqueue!");
	}
	for (UnitType t : *nextOrders) {
		// Broodwar->sendText("Enqueuing Request");
		resourceManager_->enqueueRequest(t);
	}
	delete nextOrders;
	
		

	// Give out build orders using the resource manager
	resourceManager_->useQueue();

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
	// Inform the strategy manager of the unit seen
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

	// Notify the resource manager that the resource have been spent
	if (unit->getPlayer() == Broodwar->self()) {
		resourceManager_->spentResources(unit->getType());
		// Broodwar->sendText("%s",unit->getType().getName());
	}
	
}

void TheArchitectAIModule::onUnitDestroy(BWAPI::Unit unit)
{
	// Inform the strategy manager of the unit destruction, so that it can make adjustments.
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
	// Inform the strategy manager that the unit has been completed, receiving a list of
	// new unit types to be produced.
	UnitType type = unit->getType();
	if (type == UnitTypes::Terran_SCV) {
		buildManager_->addSCV(unit);
	}
	else if (type == UnitTypes::Terran_Barracks || type == UnitTypes::Terran_Factory || type == UnitTypes::Terran_Starport || type == UnitTypes :: Terran_Command_Center) {
		productionManager_->addStructure(unit);
	}
	
}



