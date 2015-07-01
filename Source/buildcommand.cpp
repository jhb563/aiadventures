#include "buildcommand.h"

BuildCommand::BuildCommand(BWAPI::UnitType type, int priority)
	: unitToBuild_(type), priority_(priority)
{
	// Nothing else to do!
}

BWAPI::UnitType BuildCommand::getUnitType()
{
	return unitToBuild_;
}

int BuildCommand::getPriority() const
{
	return priority_;
}

//bool BuildCommand::operator() (const BuildCommand& lhs, const BuildCommand& rhs) const {
//	return lhs.getPriority() < rhs.getPriority();
//}


bool BuildCommand::canCompleteOrder(int currentMinerals, int currentGas, int supplyRoom) {
	int minsForUnit = unitToBuild_.mineralPrice();
	int gasForUnit = unitToBuild_.gasPrice();
	int supplyForUnit = unitToBuild_.supplyRequired();
	
	return minsForUnit <= currentMinerals && gasForUnit <= currentGas && supplyForUnit <= supplyRoom;
}