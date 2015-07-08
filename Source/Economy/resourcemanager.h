#ifndef __resource__manager_h
#define __resource__manager_h 1

#include <BWAPI.h>
#include <mutex>
#include <buildmanager.h>
#include <productionmanager.h>

/*
This is a class which manages all of the resources of our brood war AI

It has access to the amount of resources, such as minerals, vespene gas, and supply
that the player has access to. 

It receives requests from the BuildManager and the ProductionManager about how
to allocate those resources, informing those classes when their requests are fulfilled
and they can go ahead and begin their constructions.

It must use some kind of intelligence or strategy system to decide what order to fulfill
the requests in. 

The main AIModule, the BuildManager, and the ProductionManager must have access to this.
*/
class ResourceManager
{
public:

	
	ResourceManager(BuildManager*, ProductionManager*);
	// Compiler destructor is still fine. This class neither allocates nor deletes
	// the build manager and production manager.
	~ResourceManager() = default;

	// Called by the build manager and production manager to indicate that they
	// would like resources to build a particular type of unit.
	void enqueueRequest(BWAPI::UnitType type);

	// Called by the AI Module to inform us that resource have now been spent
	// and we can account for this in our calculations.
	void spentResources(BWAPI::UnitType type);
	
	// If the queue is not in use, begins trying to dequeue things. Otherwise exits
	void useQueue();

private:

	// Disable copy construction, default construction and assignment operator
	ResourceManager();
	ResourceManager(const ResourceManager&);
	ResourceManager& operator=(const ResourceManager&);

	// These variables reflect resources which have been allocated to be spent
	// but which have not actually been spent yet. For instance, we might allocate
	// 150 minerals to build a barracks. But until the SCV moves over to build
	// the barracks, calling BroodWar->self()->getMinerals() will not reflect this
	// allocation. So we keep track of this. 
	int mineralsAllocatedNotSpent_;
	int gasAllocatedNotSpent_;
	int supplyAllocatedNotSpent_;

	// The Queue that stores the unit types we want to build. For now, we just
	// use a deque which we'll use FIFO style (though failed requests can be put
	// back on the front)
	std::deque<BWAPI::UnitType> buildQueue_;

	// Mutex for ensuring no race conditions in the queue, or the variables
	std::mutex valuesLock;
	std::mutex queueLock;

	// Pointers to BuildManager and ProductionManager
	BuildManager* buildManager_;
	ProductionManager* productionManager_;

	// Returns true if there are enough resources to build this item
	bool canSatisfy(BWAPI::UnitType type);

	// Allocate resources for use so that we don't overspend (basically
	// increase the allocatedButNotSpent variables)
	void allocateResources(BWAPI::UnitType type);

};

#endif