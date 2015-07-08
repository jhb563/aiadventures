#include <buildmanager.h>

using namespace BWAPI;

void BuildManager::addSCV(Unit scv)
{
	// Assert scv->getType = UnitTypes::Terran_SCV
	scvs_.insert(scv);
}

bool BuildManager::buildStructure(UnitType structure)
{
	std::unordered_set<Unit>::iterator iter;
	// For now, choose any idle/mineral gathering SCV
	for (iter = scvs_.begin(); iter != scvs_.end(); ++iter) {
		Unit u = *iter;
		if (u->exists() && (u->isIdle() || u->isGatheringMinerals())) {
			TilePosition targetBuildLocation = Broodwar->getBuildLocation(structure, u->getTilePosition());
			if (targetBuildLocation)
			{
				// Order the builder to construct the supply structure
				u->build(structure, targetBuildLocation);
				return true;
			}
		}
	}
	// Couldn't find a valid SCV to build with
	return false;
}