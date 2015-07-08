#include <strategymanager.h>

using namespace std;
using namespace BWAPI;

StrategyManager::StrategyManager()
{
	timeOrders_ = list<pair<UnitType, int>>();
	timeOrders_.push_back(pair<UnitType,int>(UnitTypes::Terran_SCV,0));
	timeOrders_.push_back(pair<UnitType, int>(UnitTypes::Terran_SCV, 300));
	timeOrders_.push_back(pair<UnitType, int>(UnitTypes::Terran_SCV, 600));
	timeOrders_.push_back(pair<UnitType, int>(UnitTypes::Terran_SCV, 900));
	timeOrders_.push_back(pair<UnitType, int>(UnitTypes::Terran_SCV, 1200));

	timeOrders_.push_back(pair<UnitType, int>(UnitTypes::Terran_Supply_Depot, 1350));

	timeOrders_.push_back(pair<UnitType, int>(UnitTypes::Terran_SCV, 1500));
	timeOrders_.push_back(pair<UnitType, int>(UnitTypes::Terran_SCV, 1800));

	timeOrders_.push_back(pair<UnitType, int>(UnitTypes::Terran_Barracks, 2100));
}

list<UnitType>* StrategyManager::getOrdersForTime(int frame)
{
	// Broodwar->sendText("Getting orders while size is: %d and frame is %d", timeOrders_.size(), frame);
	list<UnitType>* results = new list<UnitType>();

	
	timeOrdersLock_.lock();
	while (!timeOrders_.empty() && timeOrders_.front().second <= frame) {
		Broodwar->sendText("Adding unit with time : %d", timeOrders_.front().second);
		results->push_back(timeOrders_.front().first);
		timeOrders_.pop_front();
	}
	timeOrdersLock_.unlock();

	return results;
}