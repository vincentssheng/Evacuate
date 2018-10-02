// Author: Sean Davis
#ifndef evacH
#define evacH

#include "EvacRunner.h"
#include <cstring>

class CityNode
{
public:
  int ID;
  int population;
  int evacuees;
  Road* roads;
  int roadCount;
  int depth;
  int potential;
  CityNode() : ID(0), population(0), evacuees(0), roads(NULL), roadCount(0), depth(0x7FFFFFFF), potential(0) {}
};

class Evac
{
public:
  int numCity;
  int numRoad;
  bool visited[10000];
  int peopleInRoad[200000];
  CityNode* adjList;
  Evac(City *cities, int numCities, int numRoads);
  void evacuate(int *evacIDs, int numEvacs, EvacRoute *evacRoutes,
                int &routeCount); // student sets evacRoutes and routeCount
  void generateDepth(int numEvacs, int* evacIDs);
  int findPath(int ID, int incomingFlow, int& routeCount, EvacRoute* evacRoutes, int hour);
  int min(int a, int b);
}; // class Evac

#endif
