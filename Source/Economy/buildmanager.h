#ifndef __build__manager__h
#define __build__manager__h 1

#include <BWAPI.h>

/*
This class manages the construction of buildings for the AI

It knows a sequence of buildings that it would like to construct
( for now. In the future it might get this order from some kind of
intelligence). It makes requests of the ResourceManager to construct
these buildings. 

It has a list of all worker units which can be used to construct
buildings. When it is notified by the ResourceManager that a request
is ready to be fulfilled, this class handles the order of constructing
the building. 

The main AIModule and ResourceManager must have access to this.
*/
class BuildManager
{
public:
	// Called in the onComplete callback of the AIModule. Adds the produced
	// SCV to our list of builders.
	void addSCV(BWAPI::Unit scvToAdd);


	// Called by the resourceManager to order us to actually build a structure.
	// Returns true if the order succeeds, false if it doesn't.
	bool buildStructure(BWAPI::UnitType type);

private:
	// The list of scvs at our disposal for building things
	std::unordered_set<BWAPI::Unit> scvs;
};

#endif