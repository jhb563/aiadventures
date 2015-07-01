#include <BWAPI.h>

class BuildCommand
{
public:

	// Basic Constructor
	BuildCommand(BWAPI::UnitType, int priority);

	// Getter for unit type
	BWAPI::UnitType getUnitType();

	// Getter for priority
	int getPriority() const;

	// bool operator() (const BuildCommand& lhs, const BuildCommand& rhs) const;

	// Comparison function (just looks at priority)
	// Input:
	//	Another BuildCommand
	//
	// Output:
	//	True if this command has a smaller priority, false otherwise
	// bool operator< (const BuildCommand& other);

	// Input:
	//	The player's current minerals
	//	The player's current gas
	//	The current "cap room" in the player's supply
	//
	// Output:
	//	If the unit/structure can be built with these materials
	bool canCompleteOrder(int currentMinerals, int currentGas, int supplyRoom);

private:
	BWAPI::UnitType unitToBuild_;
	int priority_;
};