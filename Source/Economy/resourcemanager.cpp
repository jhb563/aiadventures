
#include <resourcemanager.h>

using namespace BWAPI;

ResourceManager::ResourceManager(BuildManager* bm, ProductionManager* pm)
	: buildManager_(bm), productionManager_(pm)
{
	// Kinda hacky, but for now we initialize the "mins allocated but not spent"
	// amount to reflect the fact that we start with 4 SCVs and a CC, thus
	// the callback for onUnitCreate (which triggers us to count money as
	// spent), will decrement this value
	mineralsAllocatedNotSpent_ = 600;
	gasAllocatedNotSpent_ = 0;
	supplyAllocatedNotSpent_ = 8;
}

void ResourceManager::enqueueRequest(UnitType type)
{
	queueLock.lock();
	buildQueue_.push_back(type);
	queueLock.unlock();
}


void ResourceManager::spentResources(UnitType type)
{
	valuesLock.lock();
	mineralsAllocatedNotSpent_ -= type.mineralPrice();
	gasAllocatedNotSpent_ -= type.gasPrice();
	supplyAllocatedNotSpent_ -= type.supplyRequired();
	Broodwar->sendText("Mins : %d, Gas : %d, Supply : %d", mineralsAllocatedNotSpent_, gasAllocatedNotSpent_, supplyAllocatedNotSpent_);
	valuesLock.unlock();
}

void ResourceManager::allocateResources(UnitType type)
{
	valuesLock.lock();
	mineralsAllocatedNotSpent_ += type.mineralPrice();
	gasAllocatedNotSpent_ += type.gasPrice();
	supplyAllocatedNotSpent_ += type.supplyRequired();
	valuesLock.unlock();
}

bool ResourceManager::canSatisfy(UnitType type)
{
	valuesLock.lock();
	int mins = Broodwar->self()->minerals() - mineralsAllocatedNotSpent_;
	int gas = Broodwar->self()->gas() - gasAllocatedNotSpent_;
	int supply = Broodwar->self()->supplyTotal() - Broodwar->self()->supplyUsed() - supplyAllocatedNotSpent_;
	//Broodwar->sendText("Mins: %d, Gas : %d, Supply : %d", mins, gas, supply);
	//Broodwar->sendText("Required mins : %d, gas %d, supply %d", type.mineralPrice(), type.gasPrice(), type.supplyRequired());
	valuesLock.unlock();
	return type.mineralPrice() <= mins && type.gasPrice() <= gas && type.supplyRequired() <= supply;
}

void ResourceManager::useQueue()
{
	if (queueLock.try_lock()) {
		while (!buildQueue_.empty() && canSatisfy(buildQueue_.front())) {
			Broodwar->sendText("Conditions satisfied!");
			UnitType type = buildQueue_.front();
			
			bool built = false;
			if (type.isBuilding()) {
				built = buildManager_->buildStructure(type);
			}
			else {
				built = productionManager_->buildUnit(type);
			}
			if (built) {
				buildQueue_.pop_front();
				allocateResources(type);
			}
			
		}
		queueLock.unlock();
	}
}