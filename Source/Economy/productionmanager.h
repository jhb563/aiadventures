#ifndef __production__manager__h
#define __production__manager__h 1

#include <BWAPI.h>

/*
This is a class which handles the production of units. Based on the
number of buildings, and certain intelligence (which will be hard-coded for now),
it assembles a list of unit production requests.

It requests resources from the resource manager to service these requests. When
it is notified by the resource manager that a request has been serviced, it handles
the production of that unit. 

This class maintains a list of all unit producing structures for the AI.

The main AIModule and ResourceManager must have access to this.
*/
class ProductionManager
{
public:
	
	ProductionManager();
	~ProductionManager();

	// Adds the given structure to the appropriate list
	void addStructure(BWAPI::Unit unit);

	// Orders one of its structures to build the unit assigned.
	// Returns true if the order succeeds, false otherwise.
	bool buildUnit(BWAPI::UnitType type);


private:
	// Disable copy constructor and assignment operator
	ProductionManager(const ProductionManager&);
	ProductionManager& operator=(const ProductionManager&);

	// The list of buildings we have to build units with
	std::map<BWAPI::UnitType, std::deque<BWAPI::Unit>*> productionBuildings_;
};

#endif