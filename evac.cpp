#include <cstdlib>
#include "QueueAr.h"
#include "evac.h"
#include "EvacRunner.h"

using namespace std;

int CityNodeCompare(const void* city1, const void* city2)
{
  return (((CityNode*)city1)->potential - ((CityNode*)city2)->potential);
}

Evac::Evac(City *cities, int numCities, int numRoads)
{
  numCity = numCities;
  numRoad = 2 * numRoads;
  adjList = new CityNode[numCities];
  memset(peopleInRoad, 0, sizeof(int) * numRoad);
  for(int i = 0; i < numCity; i++)
  {
    adjList[i].ID = cities[i].ID;
    adjList[i].population = cities[i].population;
    adjList[i].evacuees = cities[i].evacuees;
    adjList[i].roadCount = cities[i].roadCount;
    adjList[i].roads = new Road[cities[i].roadCount];
    for(int j = 0; j < cities[i].roadCount; j++)
    {
      adjList[i].roads[j].destinationCityID = cities[i].roads[j].destinationCityID;
      adjList[i].roads[j].peoplePerHour = cities[i].roads[j].peoplePerHour;
      adjList[i].roads[j].ID = cities[i].roads[j].ID;
    }
  }
} // Evac()

void Evac::evacuate(int *evacIDs, int numEvacs, EvacRoute *evacRoutes,
    int &routeCount)
{
  int hour = 1;
  routeCount = 0;
  CityNode* sortedEvacs = new CityNode[numEvacs]; //Only used for sorting.
  generateDepth(numEvacs, evacIDs); //generate depth of evacs starting from center among evacs (0).
  for(int i = 0; i < numEvacs; i++) //Fill in number of evacuees in each city to be evaced.
    adjList[evacIDs[i]].evacuees = adjList[evacIDs[i]].population;
  do
  {
    for(int i = 0; i < numEvacs; i++)
    {
      if(adjList[evacIDs[i]].evacuees > 0) //If there are people left in a city to be evacuated.
      {
        int potentialFlow = 0; //Sum of all road capacities
        int count = 0;
        for(int j = 0; j < adjList[evacIDs[i]].roadCount; j++)
          if(adjList[adjList[evacIDs[i]].roads[j].destinationCityID].depth >=
             adjList[evacIDs[i]].depth)
          {
            potentialFlow += adjList[evacIDs[i]].roads[j].peoplePerHour;
            count++;
          }
        potentialFlow /= count;
        adjList[evacIDs[i]].potential = adjList[i].evacuees / potentialFlow;//Find ratio for each evac.
      }
      else //if people are all evacuated
      {
        evacIDs[i] = evacIDs[numEvacs - 1]; //Swap it with the final evac because we're done with it.
        numEvacs--; //Disregard the evac that's done in our evacIDs.
        i--; //Don't skip the newly moved evac.
      }
    }
    for(int i = 0; i < numEvacs; i++)
      sortedEvacs[i] = adjList[evacIDs[i]];
    qsort((void*)sortedEvacs, numEvacs, sizeof(CityNode), CityNodeCompare);
    for(int i = 0; i < numEvacs; i++)
      evacIDs[i] = sortedEvacs[i].ID;
    //Evacuate
    for(int i = 0; i < numEvacs; i++)
    {
      memset(visited, false, sizeof(bool) * numCity); //Initialize all cities to be unvisited.
      adjList[evacIDs[i]].evacuees -= findPath(evacIDs[i], adjList[evacIDs[i]].evacuees,
                                               routeCount, evacRoutes, hour);
    }
    //When this hour is done
    hour++;
    memset(peopleInRoad, 0, sizeof(int) * numRoad);
    //Reset the number of people in roads.
  } while(numEvacs > 0);
}

int Evac::findPath(int ID, int incomingEvacuees, int& routeCount, EvacRoute* evacRoutes, int hour)
{
  int flowThroughThis = 0;
  //Base case (if visited) = jump to return flowThroughThis
  if(!visited[ID]) //if we have not visited this city (recursive case)
  {
    visited[ID] = true;
    if(adjList[ID].depth != 0) //if current city is not an evac city.
    {
      if(incomingEvacuees > (adjList[ID].population - adjList[ID].evacuees)) //if there will be leftovers
      {
        flowThroughThis += (adjList[ID].population - adjList[ID].evacuees);
        adjList[ID].evacuees = adjList[ID].population;
      }
      else //if there will not be leftovers (second base case)
      {
        adjList[ID].evacuees += incomingEvacuees;
        return incomingEvacuees;
      }
    }
    for(int i = 0; i < adjList[ID].roadCount && flowThroughThis < incomingEvacuees; i++)
    { //For every neighbor, move people out.
      if(adjList[adjList[ID].roads[i].destinationCityID].depth >= adjList[ID].depth) //if neighbor may be a deeper city
      {
        int flowThroughSubsequent = findPath(adjList[ID].roads[i].destinationCityID,
                                             min(adjList[ID].roads[i].peoplePerHour - peopleInRoad[adjList[ID].roads[i].ID],
                                                 incomingEvacuees - flowThroughThis),
                                             routeCount, evacRoutes, hour);
        if(flowThroughSubsequent > 0)
        {
          flowThroughThis += flowThroughSubsequent;
          evacRoutes[routeCount].roadID = adjList[ID].roads[i].ID;
          evacRoutes[routeCount].time = hour;
          evacRoutes[routeCount].numPeople = flowThroughSubsequent;
          peopleInRoad[adjList[ID].roads[i].ID] += flowThroughSubsequent;
          routeCount++;
        }
      }
    }
  }
  return flowThroughThis;
}

void Evac::generateDepth(int numEvacs, int* evacIDs)
{
  Queue<int>* q = new Queue<int>(numCity);
  for(int i = 0; i < numEvacs; i++)
  {
    adjList[evacIDs[i]].depth = 0;
    q->enqueue(evacIDs[i]);
  }
  while(!q->isEmpty())
  {
    int ID = q->dequeue();
    for(int i = 0; i < adjList[ID].roadCount; i++)
    {
      if(adjList[adjList[ID].roads[i].destinationCityID].depth == 0x7FFFFFFF)
      {
        adjList[adjList[ID].roads[i].destinationCityID].depth = adjList[ID].depth + 1;
        q->enqueue(adjList[ID].roads[i].destinationCityID);
      }
    }
  }
}

int Evac::min(int a, int b)
{
  if(a < b)
    return a;
  else
    return b;
}
