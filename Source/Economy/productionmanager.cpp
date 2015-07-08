#include <productionmanager.h>

using namespace BWAPI;

ProductionManager::ProductionManager()
{
	productionBuildings_ = std::map<UnitType, std::deque<Unit>*>();

	// Initialize empty deques for each of our producing structures.
	UnitType ccType = UnitTypes::Terran_Command_Center;
	UnitType barracksType = UnitTypes::Terran_Barracks;
	UnitType factoryType = UnitTypes::Terran_Factory;
	UnitType starportType = UnitTypes::Terran_Starport;

	productionBuildings_[ccType] = new std::deque<Unit>();
	productionBuildings_[barracksType] = new std::deque<Unit>();
	productionBuildings_[factoryType] = new std::deque<Unit>();
	productionBuildings_[starportType] = new std::deque<Unit>();
}

ProductionManager::~ProductionManager()
{
	std::map<UnitType, std::deque<Unit>*>::iterator iter;
	for (iter = productionBuildings_.begin(); iter != productionBuildings_.end(); ++iter) {
		delete iter->second;
	}
}

void ProductionManager::addStructure(Unit structure)
{
	// Assert either cc, rax, fac, or port
	std::map<UnitType, std::deque<Unit>*>::iterator iter;
	iter = productionBuildings_.find(structure->getType());
	if (iter != productionBuildings_.end()) {
		iter->second->push_back(structure);
	}
}

bool ProductionManager::buildUnit(UnitType type)
{
	UnitType buildType = type.whatBuilds().first;
	std::map<UnitType, std::deque<Unit>*>::iterator iter;
	iter = productionBuildings_.find(buildType);
	if (iter != productionBuildings_.end()) {
		std::deque<Unit>* structList = iter->second;
		std::deque<Unit>::iterator structIter = structList->begin();
		while (structIter != structList->end()) {
			Unit thisStructure = *structIter;
			return thisStructure->train(type);
			++structIter;
		}
	}
	return false;
}
