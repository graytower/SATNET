#ifndef __NODE_H__
#define __NODE_H__

#include <iostream>
#include <cstdlib>
#include <vector>
#include <algorithm>

#include "topo.h"

using namespace std;

#define SET(a,_a) a = _a
#define GET(a) return a
#define ACQ(a) return &a
#define RANDOM(a,b) rand()/((double)RAND_MAX/(b-a))+a 

class Node{
public:
	Node(){}
	~Node(){}
	// Node(int _id,NodePos _pos){SET(id,_id);SET(pos,_pos);}

	int getId(){GET(id);}
	Position getPos(){GET(pos);}

	void setId(int _id){SET(id,_id);}
	void setPos(NodePos _pos){SET(pos,_pos);}
	void setName(string _name){SET(name,_name);}
	void setBrName(string _br_name){SET(br_name,_br_name);}

	virtual void nodeInitialize() = 0;
	virtual void nodeFinalize() = 0;
	virtual void linkInitialize() = 0;
	virtual void linkFinalize() = 0;
	virtual void updateLink() = 0;
	virtual void routeInitialize() = 0;

protected:
	int id;
	NodePos pos;
	string name;
	string br_name;
};

class SatNode:public Node{
public:
	SatNode();

	void nodeInitialize();
	void nodeFinalize();
	void linkInitialize();
	void linkFinalize();
	void updateLink();
	void routeInitialize();



	void updatePos();
	void setInfData(InfData data,int i){SET(infData[i],data);}
	InfData* acqInfData(int i){ACQ(infData[i]);}
    int getOrbitId(int id);
    int getOrbitIndex(int id); 
    int getForeSatelliteId(int id);
    int getRearSatelliteId(int id);
 	int getSideSatelliteId(int id,bool isEast,bool isNorth);
	void Routing_table(int src_id,StaticTopo topo,ISDB Int_DB[]);
    bool getInfStat(int infId);
    arg_route* getRouteState(int i){ACQ(RS.route_link[i]);}
    void setRouteState(arg_route route_link,int i){SET(RS.route_link[i],route_link);}

private:
	InfData infData[6];
    unsigned int sup_array[SAT_NUM][SAT_NUM];
    RouteState RS; 
};

class HostNode:public Node{
public:
	HostNode();

	void nodeInitialize();
	void nodeFinalize();
	void linkInitialize();
	void linkFinalize();
	void updateLink();
	void routeInitialize();


	void updatePos();
	void setInfData(InfData data,int i){SET(infData[i],data);}
	InfData* acqInfData(int i){ACQ(infData[i]);}
    int getOrbitId(int id);
    int getOrbitIndex(int id); 
    int getForeSatelliteId(int id);
    int getRearSatelliteId(int id);
 	int getSideSatelliteId(int id,bool isEast,bool isNorth);
	void Routing_table(int src_id,StaticTopo topo,ISDB Int_DB[]);
    bool getInfStat(int infId);

private:
	InfData infData[6];
    unsigned int sup_array[SAT_NUM][SAT_NUM];
};

#endif