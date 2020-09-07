#include "node.h"
#include "common.h"
#include "cmd.h"
#include "timer.h"
#include "route.h"

#include <fstream>
using namespace std;

SatNode::SatNode()
{
	name = SAT_NAME;
	br_name = SAT_BR_NAME;
}

void SatNode::nodeInitialize()
{
	if(ENABLE_PHYSICAL){
		int addr = id * 256*256*256 + 2;
		add_node_namespace(name.c_str(),br_name.c_str(),id,addr,8);
		//print_VethIp(name.c_str(),id,6);

	}
	setPos(ReadLocFile(id,0));
}

void SatNode::nodeFinalize()
{
	if(ENABLE_PHYSICAL){
		del_node_namespace(name.c_str(),br_name.c_str(),id);
	}
}

void SatNode::linkInitialize()
{
	if(ENABLE_PHYSICAL){
		for(int i = 0; i < SINF_NUM; i++){
			int linkId = infData[i].linkId;
			Int_DB[id-1].linkId[i]=infData[i].linkId;
			IslNode *nodes = G.isl[linkId-1].endpoint;
			if(linkId != -1 && id == nodes[0].nodeId){
				int nIds[2] = {id, nodes[1].nodeId};
				int pIds[2] = {i, nodes[1].inf};
				int addrs[2] = {nodes[0].ip,nodes[1].ip};
				//add_linkId
				add_satsat_link(name.c_str(),nIds,pIds,addrs,24,linkId);
			}
			//Int_DB initial 
		    if(linkId!=-1)
		    {
			    if(G.isl[infData[i].linkId-1].endpoint[0].nodeId==id)
			    {  
			        Int_DB[id-1].dst_satid[i]=G.isl[infData[i].linkId-1].endpoint[1].nodeId;  
		            Int_DB[id-1].dst_portid[i]=G.isl[infData[i].linkId-1].endpoint[1].inf;
			    }
			    else
			    {
				Int_DB[id-1].dst_satid[i]=G.isl[infData[i].linkId-1].endpoint[0].nodeId;
				Int_DB[id-1].dst_portid[i]=G.isl[infData[i].linkId-1].endpoint[0].inf;
			    }
		    }
		}
	}
}

void SatNode::linkFinalize()
{
	if(ENABLE_PHYSICAL)
	{
		
		del_sathost_link(SAT_BR_NAME,id,0);
		
	}
}

void SatNode::updateLink()
{

	string logFile = "log.txt";
	ofstream fon(logFile,ios::app);
	for(int i = 0; i < SINF_NUM; i++){
		bool curStat = getInfStat(i);
		if(infData[i].stat != curStat && (GetTime()<10 || GetTime()>150))
		{
			infData[i].stat = curStat;
			string buf = curStat ? " [UP] " : " [DOWN] ";
			cout << buf << id << " inf:" << i << endl;
			fon<< GetTime()<<"s "<<buf << id << " inf:" << i << endl;

			if(ENABLE_PHYSICAL){
				change_LinkState(name.c_str(),id,i,curStat);
			}

			
			
		}
		Int_DB[id-1].port_stat[i]=curStat;
		
	}

	fon.close();
	
	if(GetTime()>10 && GetTime()<=154 &&(GetTime()-10)==(id-1)*3+1)
	{
		Routing_table(id,G,Int_DB);			
	}
	else if(GetTime()>154  && GetTime()%15==0)
	{
		
		Routing_table(id,G,Int_DB);	
	}
}
void SatNode::routeInitialize()
{
	arg_route args;
	for (int i = 0; i < SAT_NUM; ++i)
	{

    	args.src_id=getId();
		args.gw_addr=-1;
		args.dst_addr=-1;
		args.dst_id=-1;
		args.src_portid=-1;
		args.linkid= -1;
    	setRouteState(args,i);
	}
}

void SatNode::updatePos()
{
	setPos(ReadLocFile(id,GetTime()));
}

int SatNode::getOrbitId(int id)
{
	return (id-1)/8 + 1;
}


int SatNode::getOrbitIndex(int id)
{
	return (id-1)%8 + 1;
}

int SatNode::getForeSatelliteId(int id)
{
	int orbitId = getOrbitId(id);
	int index = getOrbitIndex(id);
	return (orbitId - 1)*8 + index % 8 + 1;
}

int SatNode::getRearSatelliteId(int id)
{
	int orbitId = getOrbitId(id);
	int index = getOrbitIndex(id);
	
	return (orbitId - 1)*8 + (index + 6)%8 + 1;
}

int SatNode::getSideSatelliteId(int id,bool isEast,bool isNorth)
{
	int orbitId = getOrbitId(id);
	int index = getOrbitIndex(id);
	int v = -1;

	if(orbitId == 1){
		if(isEast){
			if(isNorth)
				v = orbitId*8 + (index + 6)% 8;
			else
				v = orbitId*8 + index-1;
		}
	}else if(orbitId == 6){
		if(!isEast){
			if(isNorth)
				v = (orbitId - 2)*8 + index % 8;
			else
				v = (orbitId - 2)*8 + index-1;			
		}
	}else{
		if(isEast){
			if(isNorth)
				v = orbitId*8 + (index + 6)% 8;
			else
				v = orbitId*8 + index-1;
		}else{
			if(isNorth)
				v = (orbitId - 2)*8 + index % 8;
			else
				v = (orbitId - 2)*8 + index-1;	
		}
	}

	return v + 1;
}

bool SatNode::getInfStat(int infId)
{
	int srcNodeId = id;
	int dstNodeId = -1;
	Isl isl = G.isl[ infData[infId].linkId-1 ];
	for(int i = 0; i < 2; i++)
	{
		if(srcNodeId == isl.endpoint[i].nodeId)
		{
			dstNodeId = isl.endpoint[(i+1)%2].nodeId;
			break;
		}
	}

	if(dstNodeId == getForeSatelliteId(srcNodeId) ||
		dstNodeId == getRearSatelliteId(srcNodeId))
	{
		return true;
	}
	else if(dstNodeId == getSideSatelliteId(srcNodeId, true, pos.isNorth) ||
		dstNodeId == getSideSatelliteId(srcNodeId, false, pos.isNorth)) 
	{
		return (abs(pos.loc.lat) < TOPO_BETA) && 
				(abs(sats[dstNodeId-1].getPos().loc.lat) < TOPO_BETA);
	}
	else
	{
		return false;
	}
}

void SatNode::Routing_table(
    int src_id,
    StaticTopo topo,
    ISDB Int_DB[]
)
{
	//cout<<"route"<<endl;
    int path[SAT_NUM],next_hop[SAT_NUM];
    unsigned int distance[SAT_NUM];  
    //variable for route_write
    int src_portid,dst_id,linkid;
    //int dst_linkid;
    //int gw_id;
    NodeAddress gw_addr,dst_addr;
    //support array_weight_update
    for(int i=0;i<SAT_NUM;i++)
    {
        for(int j=0;j<SAT_NUM;j++)
        {
			int temp_link = topo.arcs[i][j].linkId;
			int k =0;
			if(temp_link!=-1&&temp_link!=0)
			{
				//Route_Link[temp_link] = false;
			if(topo.isl[temp_link-1].endpoint[0].nodeId==i+1)
			    k=topo.isl[temp_link-1].endpoint[0].inf;

			else
			    k=topo.isl[temp_link-1].endpoint[1].inf;
            if(Int_DB[i].port_stat[k]==1)
            sup_array[i][j]=topo.arcs[i][j].weight;
			else 
			sup_array[i][j]=MAX_ARC_WEIGHT;
			}
			else
			sup_array[i][j]=MAX_ARC_WEIGHT;       
        }
    }

    Dijkstra(sup_array,src_id,distance,path);

	struct arg_route args[SAT_NUM];
    for(int i=0;i<SAT_NUM;i++)
    {
		//find next_hop
		next_hop[i]=FindPreNode(path,src_id,i+1);
        //thread

        if(i!=src_id-1 && next_hop[i]!=-1)
        {
            int temp=next_hop[i];
            //gw_id=temp+1;
            dst_id=i+1;
            dst_addr =dst_id*256*256*256;
            linkid=topo.arcs[src_id-1][temp].linkId;
            
            if(linkid != -1)
            {
                if(topo.isl[linkid-1].endpoint[0].nodeId==src_id)
                {
                     src_portid=topo.isl[linkid-1].endpoint[0].inf;
                     //gw_portid=topo.isl[linkid-1].endpoint[1].inf;
                     gw_addr = topo.isl[linkid-1].endpoint[1].ip;
                }
                else
                {
                     src_portid=topo.isl[linkid-1].endpoint[1].inf;
                     //gw_portid=topo.isl[linkid-1].endpoint[0].inf;
                     gw_addr = topo.isl[linkid-1].endpoint[0].ip;
                }

                int tempId = getRouteState(dst_id-1)->src_portid;
                //cout<<"dstid"<<dst_id<<"srcportid"<<src_portid<<endl;
          
                if(tempId != src_portid)
                {
                	
                	args[i].src_id=src_id;
					args[i].gw_addr=gw_addr;
					args[i].dst_addr=dst_addr;
					args[i].dst_id=dst_id;
					args[i].src_portid=src_portid;
					args[i].linkid= linkid;
                	setRouteState(args[i],dst_id-1);

					route_write("sat",src_id,src_portid,dst_addr,gw_addr);
					link_route_write("sat",src_id,src_portid,dst_id,Int_DB,gw_addr);				
                }
				
            }
        }
    }
  
}


//--------------------------------------------------------------------
HostNode::HostNode()
{
	name = HOST_NAME;
	br_name = HOST_BR_NAME;
}

void HostNode::nodeInitialize()
{
	if(ENABLE_PHYSICAL){
		int addr = id*256*256*256  + 3;
		add_node_namespace(name.c_str(),br_name.c_str(),id,addr,8);
	}
	// initialize position
	setPos(RandomPos());
	Coord loc = {0,0};
	NodePos pos = {loc,false};
	setPos(pos);
}

void HostNode::nodeFinalize()
{
	if(ENABLE_PHYSICAL){
		del_node_namespace(name.c_str(),br_name.c_str(),id);
		del_bridge(br_name.c_str(),id);		
	}
}

void HostNode::linkInitialize()
{
	if(ENABLE_PHYSICAL){
	
		add_sathost_link(SAT_BR_NAME,id,0,HOST_BR_NAME,id,0);
		}	
}

void HostNode::linkFinalize()
{
	if(ENABLE_PHYSICAL)
	{
		
		del_sathost_link(HOST_BR_NAME,id,0);
		
	}
}



void HostNode::updateLink()
{
	//
}
void HostNode::routeInitialize()
{

}


/******************************API Funcs***********************************/
bool Compare(LinkState a, LinkState b)
{
	return a.distance < b.distance;
}
