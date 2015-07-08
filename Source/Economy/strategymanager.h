#ifndef __strategy__manager__h
#define __strategy__manager__h 1

#include <BWAPI.h>
#include <mutex>

class StrategyManager
{
public:

	// The default Constructor and destructor
	StrategyManager();
	~StrategyManager() = default;

	

	// Called by the AI module when it knows that there is a pending order.
	// This method returns a list of orders which should be released to the
	// resrouce manager at (or before) the given time.
	std::list<BWAPI::UnitType>* getOrdersForTime(int frame);

private:
	// Disable copy construction and assignment operator
	StrategyManager(const StrategyManager&);
	StrategyManager& operator=(const StrategyManager&);


	// The list of orders which depend on time, and the lock for it
	std::mutex timeOrdersLock_;
	std::list<std::pair<BWAPI::UnitType, int>> timeOrders_;
};

#endif