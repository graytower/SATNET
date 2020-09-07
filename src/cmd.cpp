#include "cmd.h"
#include "common.h"
#include "timer.h"
#define runCmd_screen(m) run_cmd(m,"")
#define runCmd_noReply(m) run_cmd(m,"1> /dev/null 2> /dev/null") 
#define runCmd_toLog(m) run_cmd(m,"1> output_normal.txt 2> output_error.txt") 

void run_cmd(char *_cmd,const char *attr)
{
	char cmd[BUF_STR_SIZE];
	sprintf(cmd,"%s %s",_cmd,attr);
	if(system(cmd) < 0){
		printf("%s error:%s\n",cmd,strerror(errno));
	}
}

void add_namespace(const char *name,int id)
{
	char cmd[BUF_STR_SIZE];
	sprintf(cmd,"ip netns add %s%d",name,id);
	runCmd_noReply(cmd);
	sprintf(cmd,"ip netns exec %s%d ip link set dev lo up",name,id);
	runCmd_noReply(cmd);
	exchange_up(name,id);
}

void del_namespace(const char *name,int id)
{
	char cmd[BUF_STR_SIZE];
	sprintf(cmd,"ip netns del %s%d",name,id);
	runCmd_noReply(cmd);
}

void add_bridge(const char *name,int id)
{
	char cmd[BUF_STR_SIZE];
	sprintf(cmd,"brctl addbr %s%d",name,id);
	runCmd_noReply(cmd);
	sprintf(cmd,"brctl stp %s%d on",name,id);
	runCmd_noReply(cmd);
	sprintf(cmd,"ip link set dev %s%d up",name,id);
	runCmd_noReply(cmd);
}

void del_bridge(const char *name,int id)
{
	char cmd[BUF_STR_SIZE];
	sprintf(cmd,"ip link set dev %s%d down",name,id);
	runCmd_noReply(cmd);
	sprintf(cmd,"brctl delbr %s%d",name,id);
	runCmd_noReply(cmd);
}


void addLink_BrBr(
	const char *name1,int id1,int portId1,
	const char *name2,int id2,int portId2)
{
	char cmd[BUF_STR_SIZE];
	sprintf(cmd,"ip link add %s%dp%d type veth peer name %s%dp%d",
		name1,id1,portId1,name2,id2,portId2);
	runCmd_noReply(cmd);

	sprintf(cmd,"brctl addif %s%d %s%dp%d",
		name1,id1,name1,id1,portId1);
	runCmd_noReply(cmd);

	sprintf(cmd,"brctl addif %s%d %s%dp%d",
		name2,id2,name2,id2,portId2);
	runCmd_noReply(cmd);
}

void delLink_BrBr(const char *name,int id,int portId)
{
	char cmd[BUF_STR_SIZE];
	sprintf(cmd,"ip link del %s%dp%d",name,id,portId);
	runCmd_noReply(cmd);
}

void addLink_NsBr(
	const char *nsName,int nsId,int nsPortId,
	const char *brName,int brId,int brPortId)
{
	char cmd[BUF_STR_SIZE];
	sprintf(cmd,"ip link add %s%dp%d type veth peer name %s%dp%d",
		brName,brId,brPortId,nsName,nsId,nsPortId);
	runCmd_noReply(cmd);

	sprintf(cmd,"brctl addif %s%d %s%dp%d",
		brName,brId,brName,brId,brPortId);
	runCmd_noReply(cmd);

	sprintf(cmd,"ip link set %s%dp%d netns %s%d",
		nsName,nsId,nsPortId,nsName,nsId);
	runCmd_noReply(cmd);

	sprintf(cmd,"ip link set dev %s%dp%d up",
		brName,brId,brPortId);
	runCmd_noReply(cmd);

	sprintf(cmd,"ip netns exec %s%d ip link set dev %s%dp%d up",
		nsName,nsId,nsName,nsId,nsPortId);
	runCmd_noReply(cmd);
}

void addLink_NsNs(
	const char *nsName1,int nsId1,int pId1,
	const char *nsName2,int nsId2,int pId2)
{
	char cmd[BUF_STR_SIZE];
	sprintf(cmd,"ip link add %s%dp%d type veth peer name %s%dp%d",
		nsName1,nsId1,pId1,nsName2,nsId2,pId2);
	runCmd_noReply(cmd);

	const char *name[2] = {nsName1,nsName2};
	int nsId[2] = {nsId1,nsId2};
	int pId[2] = {pId1,pId2}; 

	for(int i = 0; i < 2; i++){
		sprintf(cmd,"ip link set %s%dp%d netns %s%d",
			name[i],nsId[i],pId[i],name[i],nsId[i]);
		runCmd_noReply(cmd);

		// sprintf(cmd,"ip netns exec %s%d ip link set dev %s%dp%d up",
		// 	name[i],nsId[i],name[i],nsId[i],pId[i]);
		// runCmd_noReply(cmd);
	}	
	//tc set 100 ms delay
	for(int i = 0; i < 2; i++)
	{
		sprintf(cmd,"ip netns exec %s%d tc qdisc add dev %s%dp%d root netem delay 100ms",
			name[i],nsId[i],name[i],nsId[i],pId[i]);
		runCmd_noReply(cmd);
	}
}

void set_VethIp(
	const char *name,int id,int portId,
	const char *addr,const char *brAddr)
{
	char cmd[BUF_STR_SIZE];
	sprintf(cmd,"ip netns exec %s%d ip addr add %s broadcast %s dev %s%dp%d",
		name,id,addr,brAddr,name,id,portId);
	runCmd_noReply(cmd);	
}

void change_LinkState(
	const char *name,int id,
	int portId,bool state)
{
	char stat[BUF_STR_SIZE];
	if(state){
		sprintf(stat,"up");
	}
	else{
		sprintf(stat,"down");
	}

	char cmd[BUF_STR_SIZE];
	if(strcmp(name,HOST_BR_NAME) == 0 || 
		strcmp(name,SAT_BR_NAME) == 0)

	{
		sprintf(cmd,"ip link set dev %s%dp%d %s",name,id,portId,stat);
	}
	if(strcmp(name,SAT_NAME) == 0 || 
		strcmp(name,HOST_NAME) == 0)
	{
		sprintf(cmd,"ip netns exec %s%d ip link set dev %s%dp%d %s",
			name,id,name,id,portId,stat);
	}
	else{
		printf("Wrong name!\n");
	}
	
	runCmd_noReply(cmd);
}


void add_node_namespace(
	const char *name,const char *br_name,
	int id,int addr,int maskSize)
{
	int portId = 0;
	add_namespace(name,id);
	add_bridge(br_name,id);
	if(strcmp(name,SAT_NAME) == 0) portId = 6;
	if(strcmp(name,HOST_NAME) == 0) portId = 1;

	addLink_NsBr(name,id,portId,br_name,id,1);

	char ipStr[BUF_STR_SIZE], bIpStr[BUF_STR_SIZE];
	IpStr(ipStr,addr,maskSize);
	BIpStr(bIpStr,addr,maskSize);
	set_VethIp(name,id,portId,ipStr,bIpStr);
}

void del_node_namespace(const char *name,const char *br_name,int id)
{
	del_namespace(name,id);
	del_bridge(br_name,id);
}

void add_satsat_link(
	const char *sname,int ids[],int pIds[],
	int addrs[],int maskSize,int linkId)
{
	addLink_NsNs(sname,ids[0],pIds[0],sname,ids[1],pIds[1]);

	char ipStr[BUF_STR_SIZE], bIpStr[BUF_STR_SIZE];

	for(int i = 0; i < 2; i++){
		IpStr(ipStr,addrs[i],maskSize);
		BIpStr(bIpStr,addrs[i],maskSize);
		set_VethIp(sname,ids[i],pIds[i],ipStr,bIpStr);
		print_VethIp(sname,ids[i],pIds[i]);
	}
}

void add_sathost_link(
	const char *name1,int id1,int portId1,
	const char *name2,int id2,int portId2)
{
	// addLink_NsBr(name1,id1,portId1,HOST_BR_NAME,id1,0);
	// addLink_NsBr(name2,id2,portId2,HOST_BR_NAME,id2,1);
	addLink_BrBr(name1,id1,portId1,name2,id2,portId2);
}

void del_sathost_link(
	const char *name,int id,int portId)
{

	delLink_BrBr(name,id,portId);
	del_bridge(name,id);
}



void print_LinkState(const char *name,int id,int portId)
{
	char cmd[BUF_STR_SIZE];
	sprintf(cmd,"ip link | grep %s%dp%d@",name,id,portId);
	runCmd_noReply(cmd);
}

void print_VethIp(const char *name,int id,int portId)
{
	char cmd[BUF_STR_SIZE];
	sprintf(cmd,"ip netns exec %s%d ip addr | grep 'global %s%dp%d'"
		,name,id,name,id,portId);
	runCmd_noReply(cmd);	
}

void IpStr(char *buf,int addr,int maskSize)
{
	int a1 = (addr & 0xff000000) >> 24;
	int a2 = (addr & 0x00ff0000) >> 16;
	int a3 = (addr & 0x0000ff00) >> 8;
	int a4 = (addr & 0x000000ff);
	sprintf(buf,"%d.%d.%d.%d/%d",a1,a2,a3,a4,maskSize);
}

void IpStr(char *buf,int addr)
{
	int a1 = (addr & 0xff000000) >> 24;
	int a2 = (addr & 0x00ff0000) >> 16;
	int a3 = (addr & 0x0000ff00) >> 8;
	int a4 = (addr & 0x000000ff);
	sprintf(buf,"%d.%d.%d.%d",a1,a2,a3,a4);
}

void BIpStr(char *buf,int addr,int maskSize)
{
	int tmp = 0;
	for(int i = 0; i < 32-maskSize;i++){
		tmp <<= 1;
		tmp |= 0x00000001;
	}
	addr |= tmp;
	int a1 = (addr & 0xff000000) >> 24;
	int a2 = (addr & 0x00ff0000) >> 16;
	int a3 = (addr & 0x0000ff00) >> 8;
	int a4 = (addr & 0x000000ff);
	sprintf(buf,"%d.%d.%d.%d",a1,a2,a3,a4);
}



void exchange_up(const char *name,int src_id)
{
	char cmd[BUF_STR_SIZE];
	sprintf(cmd,"ip netns exec %s%d sysctl net.ipv4.conf.all.forwarding=1",name,src_id);
	runCmd_noReply(cmd);
}

void link_route_write(
	const char *name,int src_id,int src_portid,int dst_id,ISDB Int_DB[],NodeAddress gw_addr)
{
	int temp_link;
	for(int p=0;p<SINF_NUM;p++)
	{
		if( Int_DB[dst_id-1].dst_satid[p]!=src_id)
		{
			temp_link=Int_DB[dst_id-1].linkId[p];
			NodeAddress temp_link_subnetIp =  G.isl[temp_link-1].subnetIp;
			char dstlink_netStr[BUF_STR_SIZE],gw_netStr[BUF_STR_SIZE];
			IpStr(dstlink_netStr,temp_link_subnetIp,24);
			IpStr(gw_netStr,gw_addr);

			char cmd[BUF_STR_SIZE],cmd_del[BUF_STR_SIZE];
			sprintf(cmd_del,"ip netns exec %s%d ip route del %s",name,src_id,dstlink_netStr);
			runCmd_noReply(cmd_del);
			sprintf(cmd,"ip netns exec %s%d ip route add %s via %s dev %s%dp%d",
			name,src_id,dstlink_netStr,gw_netStr,name,src_id,src_portid);
			runCmd_noReply(cmd);

		}
	}

}

 void route_write(
	const char *name,int src_id,int src_portid,NodeAddress dst_addr,NodeAddress gw_addr) 
{

	char dstlink_netStr[BUF_STR_SIZE],gw_netStr[BUF_STR_SIZE];
	IpStr(dstlink_netStr,dst_addr,24);
	IpStr(gw_netStr,gw_addr);
	char cmd[BUF_STR_SIZE],cmd_del[BUF_STR_SIZE];
	sprintf(cmd_del,"ip netns exec %s%d ip route del %s",name,src_id,dstlink_netStr);
	runCmd_noReply(cmd_del);

	sprintf(cmd,"ip netns exec %s%d ip route add %s via %s dev %s%dp%d",
		name,src_id,dstlink_netStr,gw_netStr,name,src_id,src_portid);
	runCmd_noReply(cmd);

}
